# 🚀 Traffic Light

## 📋 1. Overview

Task: Viết OS quản lý tín hiệu đèn giao thông với yêu cầu:
    Auto: đèn xanh 26s đèn vàng 4s đèn đỏ 30s
    Manual: nhấp nháy đèn vàng chu kì 2s
    Chuyển task bằng cách nhấn nút
---

2. Kết nối phần cứng:
    MCU stm32 blue pill (stm32f103c8t6)
    Pa0 cấu hình ngắt ngoài nhận tín hiệu từ nút nhấn (limit switch)
    Pa1 đèn đỏ 1
    Pa2 đèn vàng 1
    Pa3 đèn xanh 1
    Pa4 đèn đỏ 2
    Pa5 đèn vàng 2
    Pa6 đèn xanh 2

3. Các file trong project:
    main.c: chứa chương trình thực thi của các task và các hàm xử lý ngắt
    main.h: chứa các thư viện dùng trong project và các nguyên mẫu hàm
    OSkernel.c: chứa các chương trình thực thi của 1 os cơ bản
    OSkernel.h: chứa các thư viện cần dùng và nguyên mẫu hàm
    OSkernel.s (assembly): chứa chương trình khởi chạy task đầu tiên của hệ thống và chương trình sao lưu, khôi phục, chuyển các task

4. Các bước để build OS cơ bản:
    B1: tạo vùng stack lưu trữ các chương trình thực thi trong hệ thống
    B2: cấu hình systick để tạo thời gian quanta time mong muốn giữa các task
    B3: tạo kiểu tcb để quản lý các thread có trong hệ thống bao gồm 2 thành phần chính stackPt
        và nextPt trỏ đến stackPt của thread tiếp theo
    B4: Add các task vào os
    B5: Khởi tạo vùng stack cho task đó
    B6: khởi chạy task đầu tiên
    B7: thực hiện chuyển task

## 🎥 5. Demo
Watch a demonstration in action: https://youtu.be/HqQcPesojTo

## 🧑‍💻 6. Author

**Nguyen Hoang Minh Triet** – Final Year Student at **HCM University of Technology**  
Email: [23trietminh23@gmail.com](mailto:23trietminh23@gmail.com)  
GitHub: [@nguyentriet0810](https://github.com/nguyentriet0810)  
YouTube: [Hoang Triet](https://www.youtube.com/@hoangtriet9999)

Feel free to open an issue or contact me for any questions or suggestions!










