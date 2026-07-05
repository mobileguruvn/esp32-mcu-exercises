#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_reg.h"     /* GPIO_ENABLE1_REG, GPIO_OUT1_REG (bank 1) */
#include "soc/io_mux_reg.h"   /* IO_MUX_GPIO48_REG, PIN_FUNC_SELECT, PIN_FUNC_GPIO */
#include "esp_log.h"          /* ESP_LOGI */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LED_GPIO    48U               /* GPIO pin to drive */
#define LED_BIT     (LED_GPIO - 32U)  /* GPIO48 is in BANK 1 (32..48) -> bit 16 */
#define LED_MASK    (1U << LED_BIT)   /* bit mask: shift 1 to the pin's bit position */

#define BLINK_MS    500U              /* 500ms ON + 500ms OFF = 1 second cycle */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void led_init(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const char *s_tag = "gpio";

/*******************************************************************************
 * Code
 ******************************************************************************/
static void led_init(void)
{
    /* 1) IO MUX: route the pin to the plain GPIO function */
    PIN_FUNC_SELECT(IO_MUX_GPIO48_REG, PIN_FUNC_GPIO);

    /* 2) Set pin direction to OUTPUT: set the bit using OR (|) */
    REG_WRITE(GPIO_ENABLE1_REG, REG_READ(GPIO_ENABLE1_REG) | LED_MASK);

    /* 3) Start LOW: clear the bit using AND (&) with the inverted mask (~) */
    REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) & ~LED_MASK);
}

void app_main(void)
{
    uint32_t cycle = 0U;

    led_init();

    for (;;)
    {
        /* ON: set the bit to 1 -> pin drives HIGH (OR operator) */
        REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) | LED_MASK);
        ESP_LOGI(s_tag, "[%lu] GPIO%u = ON  (HIGH)", cycle, LED_GPIO);
        vTaskDelay(pdMS_TO_TICKS(BLINK_MS));

        /* OFF: clear the bit to 0 -> pin drives LOW (AND NOT operator) */
        REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) & ~LED_MASK);
        ESP_LOGI(s_tag, "[%lu] GPIO%u = OFF (LOW)", cycle, LED_GPIO);
        vTaskDelay(pdMS_TO_TICKS(BLINK_MS));

        cycle++;
    }
}
