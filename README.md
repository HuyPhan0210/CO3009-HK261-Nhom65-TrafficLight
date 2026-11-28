# CO3009 - HK261 - Nhóm 65: Hệ Thống Điều Khiển Đèn Giao Thông 2 Chiều

Dự án Bài tập lớn môn **Vi xử lý - Vi điều khiển (CO3009)** thực hiện trên kit **XNUCLEO-F103RB** (STM32F103RBTx) sử dụng **STM32CubeIDE**.

---

## 1. Cấu trúc thư mục dự án

```text
CO3009-HK261-Nhom65-TrafficLight/
├── .gitignore                  # Bỏ qua thư mục Debug/, file biên dịch rác
├── README.md                   # Tài liệu hướng dẫn làm việc nhóm trên Git
└── TrafficLight_Nhom65/        # Thư mục dự án chính của STM32CubeIDE
    ├── TrafficLight_Nhom65.ioc # File cấu hình chân và ngoại vi (Chỉ TV1 chỉnh sửa)
    ├── Drivers/                # Thư viện HAL chuẩn của STM32 (Không chỉnh sửa)
    └── Core/
        ├── Startup/            # Mã khởi động vi điều khiển do CubeIDE sinh ra
        ├── Inc/                # Chứa toàn bộ file header (.h)
        └── Src/                # Chứa toàn bộ file mã nguồn (.c)
```

---

## 2. Phân công module & File phụ trách

Để tránh xung đột code (`merge conflict`), **mỗi thành viên chỉ tạo và chỉnh sửa đúng các file được phân công** bên trong `TrafficLight_Nhom65/Core/Inc/` và `Core/Src/`:

| Thành viên | Nhánh Git làm việc | File được phép tạo & chỉnh sửa | Nhiệm vụ chính |
| :--- | :--- | :--- | :--- |
| **TV1 (Nhóm trưởng)** | `feature/core-fsm-auto` | `.ioc`, `main.c`<br>`global.h/.c`<br>`scheduler.h/.c`<br>`software_timer.h/.c`<br>`fsm_automatic.h/.c` | Cấu hình phần cứng `.ioc`, bộ định thời phần mềm (10 ms), bộ lập lịch tác vụ và chế độ đèn tự động (`AUTOMATIC`). |
| **TV2** | `feature/button-traffic` | `button.h/.c`<br>`traffic_light.h/.c`<br>`fsm_manual.h/.c` | Đọc & chống dội nút nhấn, xuất tín hiệu 4 trạng thái đèn giao thông, chế độ chỉnh tay (`MANUAL`). |
| **TV3** | `feature/dht20-lcd` | `dht20.h/.c`<br>`lcd.h/.c`<br>`fsm_tuning.h/.c` | Giao tiếp I2C đọc nhiệt độ/độ ẩm DHT20, hiển thị thông số lên LCD, chế độ tinh chỉnh thời gian (`TUNING`). |

> **Cách tạo file mới trong STM32CubeIDE:**
> - Chuột phải vào `Core/Inc` $\rightarrow$ **New** $\rightarrow$ **Header File** (đặt tên dạng `ten_module.h`).
> - Chuột phải vào `Core/Src` $\rightarrow$ **New** $\rightarrow$ **Source File** (đặt tên dạng `ten_module.c`).

---

## 3. Quy ước đặt tên (Coding Convention)

Cả nhóm thống nhất đặt tên theo chuẩn sau để ghép nối code trơn tru:

- **Tên file:** `snake_case` viết thường toàn bộ (VD: `traffic_light.h`, `software_timer.c`).
- **Hằng số / Macro / Trạng thái FSM:** `UPPER_SNAKE_CASE` (VD: `INIT`, `AUTO_RED_GREEN`, `TIME_OUT`).
- **Tên hàm:** `ten_module_hanh_dong()` viết thường (VD: `button_reading()`, `traffic_set_way1()`, `lcd_display()`).
- **Biến toàn cục dùng chung:** Khai báo `extern` trong `global.h`, định nghĩa trong `global.c` (VD: `extern int status;`, `extern int red_duration;`).
- **User Label chân GPIO (đã cấu hình cố định trong `.ioc`):**
  - Đèn giao thông 1 (`D3`, `D4`): `TL1_A` (`PB3`), `TL1_B` (`PB5`)
  - Đèn giao thông 2 (`D5`, `D6`): `TL2_A` (`PB4`), `TL2_B` (`PB10`)
  - Nút nhấn 1 (`D7`, `D8`): `BTN_1` (`PA8`), `BTN_2` (`PA9`)
  - Nút nhấn 2 (`D9`, `D10`): `BTN_3` (`PC7`), `BTN_4` (`PB6`)
  - Nút USER trên mạch: `BTN_U` (`PC13`)
  - LED trên mạch: `LED_1` (`PA5`), `LED_2` (`PC9`)
  - Cảm biến DHT20 & LCD: `I2C1` (`PB8` - SCL, `PB9` - SDA)
  - Ngắt Timer hệ thống: `TIM2` chu kỳ `10 ms`

---

## 4. Hướng dẫn thao tác Git cho thành viên

### Bước 1: Tải dự án về máy (Chỉ làm 1 lần đầu)
Mở **Git Bash** tại thư mục muốn lưu bài (nên dùng đường dẫn ngắn, không dấu tiếng Việt, VD: `D:\CO3009\`) và chạy:

```bash
git clone [https://github.com/HuyPhan0210/CO3009-HK261-Nhom65-TrafficLight.git](https://github.com/HuyPhan0210/CO3009-HK261-Nhom65-TrafficLight.git)
cd CO3009-HK261-Nhom65-TrafficLight
git checkout dev
git checkout -b feature/ten-nhanh-cua-ban
```

### Bước 2: Mở dự án vào STM32CubeIDE
1. Mở **STM32CubeIDE** (giữ nguyên Workspace mặc định).
2. Vào **File** $\rightarrow$ **Open Projects from File System...**
3. Nhấn **Directory...**, chọn đúng thư mục con **`TrafficLight_Nhom65`** bên trong thư mục vừa clone $\rightarrow$ nhấn **Finish**.
*(Lưu ý: Tuyệt đối không bấm tạo New STM32 Project mới).*

### Bước 3: Kéo code mới nhất từ nhánh `dev` về trước khi code (Pull)
Mỗi lần bắt đầu phiên làm việc mới, chạy các lệnh sau để cập nhật code mới của các thành viên khác:

```bash
git checkout dev
git pull origin dev
git checkout feature/ten-nhanh-cua-ban
git merge dev
```

### Bước 4: Đẩy code của bạn lên GitHub (Push)
1. Bấm **Build (`Ctrl + B`)** trên STM32CubeIDE và đảm bảo báo **`0 errors`**.
2. Chạy các lệnh sau trong Git Bash tại thư mục gốc repo:

```bash
git status
git add .
git commit -m "feat: mo ta ngan gon chuc nang vua lam"
git push -u origin feature/ten-nhanh-cua-ban
```
3. Lên trang web GitHub của nhóm, bấm **Compare & pull request** để gộp code từ nhánh `feature/...` của bạn vào nhánh **`dev`**.

---

## 5. Quy tắc vàng tránh lỗi hệ thống

1. **Không sửa `.ioc` và `main.c` tự phát:** Nếu cần thêm chân hoặc sửa cấu hình ngoại vi, báo lại cho TV1 cập nhật trên `.ioc` rồi kéo về từ `dev`.
2. **Không tắt chân nạp code (`PA13`, `PA14`):** Luôn giữ chế độ `Debug: Serial Wire` để tránh khóa chip.
3. **Không xóa file `.gitignore`:** Đảm bảo thư mục `Debug/` không bị đẩy lên GitHub.
