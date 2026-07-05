# esp32-mcu-exercises

Tổng hợp các bài thực hành (exercise) trong quá trình học lập trình embedded với **ESP32-S3**.

## Môi trường

| Thành phần | Chi tiết |
|---|---|
| Board | ESP32-S3-DevKitC-1 (N16R8: 16MB flash, 8MB Octal PSRAM) |
| Framework | ESP-IDF v6.0.1 |
| IDE | VS Code + ESP-IDF extension |
| Hệ điều hành | macOS |
| Mô phỏng | Wokwi (prototyping GPIO/mạch) |

## Cấu trúc repo

\`\`\`
esp32-mcu-exercises/
├── README.md
├── .gitignore
├── 01-cisc-vs-risc/              # Báo cáo so sánh kiến trúc CISC vs RISC
│   └── CISC_vs_RISC_Comparison.md
├── exercise_03_gpio-register/    # GPIO mức thanh ghi: nhấp nháy LED
│   ├── CMakeLists.txt
│   ├── sdkconfig
│   └── main/
├── exercise_04_gpio_interrupt/   # GPIO + ngắt: nút BOOT toggle LED
│   ├── CMakeLists.txt
│   ├── sdkconfig
│   └── main/
└── ...                           # Các exercise tiếp theo
\`\`\`

Mỗi exercise nằm trong một thư mục riêng, đánh số thứ tự để dễ theo dõi tiến độ học.

## Cách build & flash một exercise (dự án ESP-IDF)

\`\`\`bash
cd <ten-thu-muc-exercise>
idf.py set-target esp32s3
idf.py menuconfig
idf.py build
ls /dev/cu.*
idf.py -p PORT flash monitor
\`\`\`

Thoát monitor: \`Ctrl + ]\`

## Danh sách exercise

- [x] **01 — So sánh kiến trúc CISC vs RISC** (báo cáo lý thuyết)
- [ ] 02 — (đang cập nhật)
- [x] **03 — GPIO mức thanh ghi** — nhấp nháy LED bằng cách ghi trực tiếp thanh ghi GPIO + toán tử bitwise (chu kỳ 1 giây), không dùng thư viện driver
- [x] **04 — GPIO với ngắt (interrupt)** — cấu hình ngắt sườn xuống cho nút BOOT (GPIO0) ở mức thanh ghi; mỗi lần nhấn, ISR đặt cờ và vòng chính bật/tắt LED

---

*Repo học tập cá nhân. Nội dung mang tính tham khảo trong quá trình học.*
