/*
 * Exercise 04 - GPIO with Interrupt (ESP32-S3, register level)
 *
 * Goal: press the BOOT button (GPIO0) -> fire an interrupt -> toggle the LED.
 *
 * Flow:
 *   1. Press BOOT: pin GPIO0 goes HIGH -> LOW (falling edge).
 *   2. GPIO hardware detects the falling edge -> calls gpio_isr().
 *   3. gpio_isr() does the minimum: sets the flag s_pressed = true.
 *   4. app_main() loop sees the flag -> toggles LED red <-> off, clears flag.
 *
 * Two parts:
 *   - BUTTON + INTERRUPT  <- focus of this exercise (configured via registers)
 *   - WS2812 LED          <- reused from exercise 3 (not the focus)
 */
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_reg.h"     /* GPIO registers: PIN0, STATUS, ENABLE, OUT1... */
#include "soc/io_mux_reg.h"   /* IO MUX: pin function select, FUN_IE */
#include "soc/interrupts.h"   /* ETS_GPIO_INTR_SOURCE (GPIO interrupt source id) */
#include "esp_intr_alloc.h"   /* esp_intr_alloc(): attach the ISR to a CPU interrupt */
#include "esp_cpu.h"          /* esp_cpu_get_cycle_count() - CPU cycle counter */
#include "esp_attr.h"         /* IRAM_ATTR */
#include "esp_log.h"          /* ESP_LOGI */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* ---- Button: BOOT on GPIO0 (bank 0) ---- */
#define BTN_GPIO          0U
#define BTN_MASK          (1U << BTN_GPIO)   /* mask for bit 0 */
#define INT_TYPE_FALLING  2U                 /* interrupt type: 2 = falling edge */
#define INT_ENA_CPU0      1U                 /* enable interrupt for CPU0 (bit 0) */

/* ---- WS2812 RGB LED on GPIO48 (bank 1) ---- */
#define LED_GPIO          48U
#define LED_BIT           (LED_GPIO - 32U)   /* bank 1 -> bit 16 */
#define LED_MASK          (1U << LED_BIT)
#define LED_RED           0x004000U          /* red (G=00, R=40, B=00) */
#define LED_OFF           0x000000U

/* WS2812 pulse lengths in CPU cycles @160MHz (just pulse widths, no need to master) */
#define WS2812_BITS       24U
#define T0H 64U
#define T0L 136U
#define T1H 128U
#define T1L 72U

#define DEBOUNCE_MS       200U                /* switch debounce time */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void button_init(void);
static void led_init(void);
static void led_show(uint32_t color);
static void gpio_isr(void *arg);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const char *s_tag = "ex04";
static portMUX_TYPE s_ledMux = portMUX_INITIALIZER_UNLOCKED;
static volatile bool s_pressed = false;       /* ISR sets this: "button was pressed" */

/*******************************************************************************
 * Code
 ******************************************************************************/
/* ===== PART 1: BUTTON + INTERRUPT (the focus) ============================= */

/* Configure GPIO0 as INPUT and enable a falling-edge interrupt - all via registers */
static void button_init(void)
{
    /* IO MUX: pin 0 = plain GPIO, and enable the input buffer (FUN_IE) */
    PIN_FUNC_SELECT(IO_MUX_GPIO0_REG, PIN_FUNC_GPIO);
    REG_SET_BIT(IO_MUX_GPIO0_REG, FUN_IE);

    /* Make sure pin 0 is NOT an output (clear bit 0 in the ENABLE register) */
    REG_WRITE(GPIO_ENABLE_W1TC_REG, BTN_MASK);

    /* Set interrupt type = falling edge (INT_TYPE field in GPIO_PIN0_REG) */
    REG_SET_FIELD(GPIO_PIN0_REG, GPIO_PIN0_INT_TYPE, INT_TYPE_FALLING);

    /* Enable GPIO0 interrupt routed to CPU0 (INT_ENA field) */
    REG_SET_FIELD(GPIO_PIN0_REG, GPIO_PIN0_INT_ENA, INT_ENA_CPU0);

    /* Clear any stale interrupt flag (write 1 to clear) */
    REG_WRITE(GPIO_STATUS_W1TC_REG, BTN_MASK);
}

/* ISR: must be very short. Just clear the flag and notify the main loop. */
static void IRAM_ATTR gpio_isr(void *arg)
{
    /* Which pin has a pending interrupt? If it's the BOOT button, handle it. */
    if ((REG_READ(GPIO_STATUS_REG) & BTN_MASK) != 0U)
    {
        REG_WRITE(GPIO_STATUS_W1TC_REG, BTN_MASK);   /* clear flag -> avoid endless re-trigger */
        s_pressed = true;                            /* tell app_main to handle it */
    }
}

/* ===== PART 2: WS2812 LED (reused from exercise 3, not the focus) ========= */

/* Configure the LED pin (GPIO48) as OUTPUT */
static void led_init(void)
{
    PIN_FUNC_SELECT(IO_MUX_GPIO48_REG, PIN_FUNC_GPIO);
    REG_WRITE(GPIO_ENABLE1_W1TS_REG, LED_MASK);   /* enable OUTPUT (bank 1) */
    REG_WRITE(GPIO_OUT1_W1TC_REG, LED_MASK);      /* start LOW */
}

/* Send 24 color bits to the WS2812 (HIGH pulse width decides bit 0/1) */
static void led_show(uint32_t color)
{
    uint32_t i     = 0U;
    uint32_t start = 0U;
    uint32_t th    = 0U;
    uint32_t tl    = 0U;

    portENTER_CRITICAL(&s_ledMux);                /* disable interrupts to keep timing */
    for (i = 0U; i < WS2812_BITS; i++)
    {
        /* Take each bit from most-significant to least-significant */
        if (((color >> (WS2812_BITS - 1U - i)) & 1U) != 0U)
        {
            th = T1H; tl = T1L;
        }
        else
        {
            th = T0H; tl = T0L;
        }

        start = esp_cpu_get_cycle_count();
        REG_WRITE(GPIO_OUT1_W1TS_REG, LED_MASK);  /* HIGH */
        while ((esp_cpu_get_cycle_count() - start) < th) { }

        start = esp_cpu_get_cycle_count();
        REG_WRITE(GPIO_OUT1_W1TC_REG, LED_MASK);  /* LOW */
        while ((esp_cpu_get_cycle_count() - start) < tl) { }
    }
    portEXIT_CRITICAL(&s_ledMux);
}

/* ===== MAIN ================================================================ */
void app_main(void)
{
    bool led_on = false;

    led_init();
    button_init();

    /* The ONLY API call: attach gpio_isr to the CPU's GPIO interrupt */
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_GPIO_INTR_SOURCE, 0, gpio_isr, NULL, NULL));

    led_show(LED_OFF);
    ESP_LOGI(s_tag, "Ready - press the BOOT button to toggle the LED");

    for (;;)
    {
        if (s_pressed)                       /* was the button pressed? */
        {
            led_on = !led_on;                /* toggle state */
            led_show(led_on ? LED_RED : LED_OFF);
            ESP_LOGI(s_tag, "BOOT pressed -> LED %s", led_on ? "ON (RED)" : "OFF");

            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));   /* wait out switch bounce */
            s_pressed = false;                        /* ignore bounce interrupts */
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
