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
├── 01-cisc-vs-risc/        # Báo cáo so sánh kiến trúc CISC vs RISC
│   └── CISC_vs_RISC_Comparison.md
└── ...                     # Các exercise tiếp theo
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

---

*Repo học tập cá nhân. Nội dung mang tính tham khảo trong quá trình học.*
