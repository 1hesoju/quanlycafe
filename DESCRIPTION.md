# TÀI LIỆU MÔ TẢ DỰ ÁN (PROJECT DESCRIPTION)
# HỆ THỐNG QUẢN LÝ NHÂN VIÊN QUÁN CÀ PHÊ
### (Coffee Shop Employee Management System - C++17 & Qt6)

---

## 1. TỔNG QUAN DỰ ÁN (PROJECT OVERVIEW)
**Hệ Thống Quản Lý Nhân Viên Quán Cà Phê** là phần mềm quản lý toàn diện dành cho các mô hình kinh doanh F&B (quán cà phê, trà sữa, chuỗi đồ uống). Hệ thống tự động hóa trọn vẹn các nghiệp vụ nhân sự đặc thù: quản lý hồ sơ đa vị trí, phân ca linh hoạt theo ngày, kiểm tra chống trùng giờ làm việc, chấm công thời gian thực thông minh và tự động tính toán bảng lương.

Dự án được xây dựng hoàn toàn bằng ngôn ngữ **C++ tiêu chuẩn C++17**, kết hợp thư viện đồ họa **Qt6** để mang lại giao diện người dùng Desktop hiện đại, mượt mà và trực quan; đồng thời tích hợp sẵn giao diện dòng lệnh **Console CLI** phục vụ thử nghiệm linh hoạt.

---

## 2. CÁC TÍNH NĂNG CHÍNH CỦA HỆ THỐNG

### 2.1. Phân hệ Quản trị viên (Admin)
- **Bảng điều hành (Dashboard):** Thống kê số lượng nhân viên theo từng trạng thái (Đang làm, Tạm nghỉ, Đã nghỉ), số ca trực diễn ra trong ngày và nhật ký hoạt động thời gian thực.
- **Quản lý Hồ sơ Nhân sự:** Thêm mới, chỉnh sửa thông tin, cập nhật chức vụ, số điện thoại, email, mức lương cơ bản và phụ cấp.
- **Tích hợp Ảnh đại diện (Avatar):** Tự động cắt tròn ảnh đại diện hiển thị trong bảng danh sách; tự động tạo Avatar ký tự viết tắt thông minh khi chưa có ảnh.
- **Bộ lọc & Tìm kiếm đa tiêu chí:** Tìm kiếm tức thì theo Mã NV, Họ tên; lọc theo loại chức vụ (Pha chế, Thu ngân,...) và trạng thái làm việc; sắp xếp theo Mã, Tên A-Z, Mức lương.
- **Quản lý Lịch làm việc & Phân ca:**
  - Tự động duy trì 3 ca cố định mỗi ngày: Ca Sáng (07:00 - 12:00), Ca Chiều (12:00 - 17:00), Ca Tối (17:00 - 22:00).
  - Hỗ trợ thêm ca bổ sung (ca gãy, ca sự kiện cuối tuần) và xóa ca bổ sung khi không sử dụng.
  - Phân ca linh hoạt: 1 nhân viên có thể làm nhiều ca trong cùng một ngày nếu các ca không bị trùng/chồng chéo giờ làm việc thực tế.
  - Thuật toán `bTrungCa` tự động kiểm tra và ngăn chặn hoàn toàn việc xếp trùng giờ.
  - Cơ chế chuẩn hóa `chuanHoaLich` bảo đảm không trùng lặp dòng dữ liệu hay mã nhân viên khi lưu.
- **Chấm công thông minh (Smart Timekeeping):**
  - Quy tắc 30 phút: Cho phép check-in sớm tối đa 30 phút trước giờ ca bắt đầu; chặn check-in ngoài khung giờ.
  - Tự động phân loại trạng thái chấm công: Đúng giờ, Đi muộn, Về sớm.
  - Kiểm tra logic vào/ra hợp lệ (phải check-in trước mới được check-out).
- **Tính lương tự động (Automated Payroll):** Tự động tính toán tổng số giờ làm thực tế trong tháng; phân biệt lương theo tháng + phụ cấp cho Full-time và lương theo giờ cho Part-time; tính tổng quỹ lương toàn quán.
- **Quản lý Tài khoản & Phân quyền:** Cấp tài khoản mới, đặt lại mật khẩu về mặc định (`123456`), bảo vệ an toàn tài khoản đang đăng nhập.

### 2.2. Phân hệ Nhân viên (Employee)
- **Hồ sơ cá nhân:** Xem thông tin cá nhân, ngày vào làm và tự tải lên ảnh đại diện cá nhân mới.
- **Lịch làm của tôi:** Tra cứu toàn bộ ca làm được phân công, tự động sắp xếp chuẩn xác theo đúng dòng thời gian (Năm → Tháng → Ngày → Giờ bắt đầu).
- **Chấm công cá nhân:** Tự bấm Check-in khi vào ca và Check-out khi kết thúc ca với trạng thái hiển thị trực quan.
- **Bảng lương cá nhân:** Tra cứu tổng số giờ làm việc thực tế và tiền lương thực lĩnh theo từng tháng.

### 2.3. Phân hệ Bán hàng & Gọi món (iPOS Order System)
- **Áp dụng cho:** Nhân viên Phục Vụ (`PHU_VU`), Thu Ngân (`THU_NGAN`), Quản Lý (`QUAN_LY`) và Quản trị viên (`Admin`).
- **Sơ đồ 30 Bàn trực quan (iPOS Style):**
  - Hiển thị lưới 30 ô bàn được đánh số từ 1 đến 30.
  - Phân loại màu sắc thời gian thực:
    - **Màu Xanh Nước Biển:** Bàn đang có khách và đã gọi món (hiển thị số lượng món).
    - **Màu Xám:** Bàn trống, chưa có khách.
    - **Viền Cam Nổi Bật:** Bàn hiện đang được người dùng bấm chọn để thao tác.
- **Thực đơn Menu trực quan:**
  - Hiển thị danh mục 8 thức uống đặc trưng của quán với đầy đủ ảnh đại diện, tên món và đơn giá:
    1. Cà Phê Muối: 20.000 đ
    2. Bạc Xỉu Kem Muối: 25.000 đ
    3. Nâu Đá: 18.000 đ
    4. Sữa Tươi Cà Phê: 20.000 đ
    5. Bạc Xỉu: 20.000 đ
    6. Đen Đá: 15.000 đ
    7. Coldbrew: 20.000 đ
    8. Trà Tắc Xí Muội: 20.000 đ
  - Thao tác 1-chạm: Bấm nút "+ Thêm" hoặc thẻ món để thêm trực tiếp vào bàn đang chọn.
- **Chi tiết & Thao tác Order:**
  - Bảng danh sách chi tiết: STT, Tên món, Đơn giá, Số lượng, Thành tiền, Trạng thái pha chế.
  - Điều chỉnh số lượng (+1, -1) hoặc xóa món khỏi bàn.
  - **Nút "Chốt Bill / Gửi Pha Chế":** Chuyển toàn bộ danh sách món vừa đặt sang quầy Pha Chế, chuyển trạng thái bàn sang Có khách (Màu xanh nước biển).
  - **Nút "Thanh Toán & Trả Bàn":** Xuất hóa đơn chi tiết, ghi nhận vào lịch sử doanh thu (`data/hoadon.csv`), tự động reset bàn về trạng thái Trống (Màu xám) khi khách rời đi.
  - **Nút "Trả Bàn":** Trả bàn về trạng thái trống khi khách hủy món.

### 2.4. Phân hệ Quầy Pha Chế (Kitchen Display System - KDS)
- **Áp dụng cho:** Nhân viên Pha Chế (`PHA_CHE`), Quản Lý (`QUAN_LY`) và Quản trị viên (`Admin`).
- **Xếp hàng món theo thời gian thực (FIFO Queue):** Hiển thị tuần tự các món cần làm theo đúng thứ tự khách gọi; hiển thị số bàn, hình ảnh món, tên món, số lượng, giờ gọi và trạng thái.
- **Quy trình tương tác thông minh:**
  - **Nút "Xác nhận":** Nhân viên pha chế bấm nhận món khi bắt đầu thực hiện (chuyển trạng thái sang "Đang pha chế").
  - **Nút "Hoàn thành":** Bấm khi đã pha chế xong món (chuyển trạng thái sang "Đã xong").
  - Tự động phát tín hiệu thông báo đến nhân viên Phục Vụ để mang món ra bàn cho khách.

### 2.5. Phân hệ Phục Vụ & Nhận thông báo bưng món (Serving System)
- **Áp dụng cho:** Nhân viên Phục Vụ (`PHU_VU`).
- **Thông báo Real-time:** Khi quầy Pha Chế bấm "Hoàn thành", hệ thống sẽ tự động bật thông báo Popup và cập nhật danh sách các món cần bưng kèm chính xác số bàn.
- **Xác nhận bưng món:** Cung cấp nút "Đã bưng ra bàn" để nhân viên phục vụ xác nhận sau khi đưa đồ uống đến bàn khách, đưa trạng thái món sang "Đã phục vụ".

### 2.6. Phân hệ Quản lý Kết ca & Thống kê Doanh thu (Shift Cash-out & Revenue)
- **Phân quyền bảo mật nghiêm ngặt:** Chỉ có **Quản trị viên (`Admin`)** và **Thu ngân (`THU_NGAN`)** mới có quyền truy cập, xem và thao tác tab này; các vai trò khác (`PHA_CHE`, `PHU_VU`, `BAO_VE`, `QUAN_LY`) hoàn toàn bị ẩn để bảo đảm an toàn tài chính.
- **Quản lý Tiền đầu ca (Tiền mặt giao ca):**
  - Cho phép Thu ngân hoặc Admin nhập số tiền mặt bắt đầu ca (tiền thối/tiền két ban đầu) cho từng ca làm việc trong ngày.
  - Tự động ghi nhận và lưu trữ vào tệp dữ liệu `data/ketca.csv`.
- **Tổng tiền kết ca theo từng ngày:**
  - Lọc và hiển thị danh sách toàn bộ các ca làm việc trong ngày được chọn kèm khung giờ tương ứng.
  - **Tự động bóc tách doanh thu thực tế:** Tổng hợp tự động các hóa đơn được thanh toán trong khung giờ của từng ca (`TienDauCa + DoanhThuCa = TongTienCuoiCa`).
  - **Chốt kết ca an toàn:** Cung cấp nút "Chốt Kết Ca", tự động ghi nhận người chốt (tên tài khoản đăng nhập), thời gian chốt, số lượng đơn hàng và chuyển trạng thái sang `ĐÃ CHỐT KẾT CA`.
  - Hộp tổng hợp tài chính ngày: Hiển thị trực quan `Tổng tiền đầu ca`, `Tổng doanh thu các ca` và `TỔNG TIỀN THU VỀ TRONG NGÀY`.
- **Tổng hợp tiền thu về trong tháng (Monthly Financial Summary):**
  - Bộ chọn Tháng/Năm linh hoạt (chống lăn chuột vô ý).
  - Thống kê chi tiết danh sách từng ngày trong tháng: Ngày, Số hóa đơn phát sinh, Tiền đầu ca, Doanh thu bán hàng, Tổng tiền trong ngày, Số ca đã chốt.
  - Tổng kết toàn tháng: Hiển thị tổng số lượng đơn hàng đã bán và **TỔNG DOANH THU THỰC TẾ THU VỀ TRONG THÁNG**.

---

## 3. KIẾN TRÚC LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG (OOP DESIGN)

### 3.1. Tính Trừu tượng (Abstraction)
Lớp trừu tượng cơ sở `NhanVien` mô hình hóa các đặc trưng chung của nhân viên. Các phương thức thuần ảo định nghĩa hành vi bắt buộc cho các lớp con:
```cpp
class NhanVien {
protected:
    std::string maNV, hoTen, sdt, email, ngayVaoLam, avatar;
    double luongCoBan;
    TrangThaiNV trangThai;
public:
    virtual double tinhLuong(double soGioLam = 0) const = 0;
    virtual std::string loaiNhanVien() const = 0;
    virtual std::string toCSV() const = 0;
};
```

### 3.2. Tính Kế thừa (Inheritance)
5 lớp dẫn xuất kế thừa công khai từ lớp cơ sở `NhanVien`:
- `PhaChe` (Barista): Chịu trách nhiệm pha chế đồ uống, tính lương theo giờ hoặc lương cơ bản.
- `ThuNgan` (Cashier): Kế thừa thuộc tính chung và mở rộng thêm thuộc tính riêng `phuCapTrachNhiem`.
- `QuanLy` (Manager): Kế thừa và mở rộng thêm thuộc tính `phuCapQuanLy`.
- `PhuVu` (Server): Nhân viên phục vụ bàn và nhận order.
- `BaoVe` (Security): Nhân viên trông giữ xe và bảo đảm an ninh quán.

### 3.3. Tính Đóng gói (Encapsulation)
Các thuộc tính nội tại của đối tượng được bảo vệ bằng phạm vi `protected` hoặc `private`. Mọi truy cập hay thay đổi giá trị đều phải thông qua các phương thức Getter và Setter có tích hợp kiểm tra tính hợp lệ dữ liệu.

### 3.4. Tính Đa hình (Polymorphism)
Toàn bộ nhân viên được quản lý tập trung thông qua danh sách con trỏ thông minh: `std::vector<std::unique_ptr<NhanVien>>`. Khi gọi các phương thức `nv->tinhLuong()` hoặc `nv->toCSV()`, cơ chế Dynamic Binding (thông qua bảng phương thức ảo `vtable`) sẽ tự động gọi chính xác phương thức được nạp chồng của lớp con tương ứng.

### 3.5. Mẫu thiết kế Factory Method
Lớp `FileManager` hiện thực mẫu thiết kế **Factory Method** trong phương thức `taoTuDong(const std::string& line)`. Khi đọc tệp dữ liệu CSV, hàm sẽ phân tích trường `loai` để tự động khởi tạo đúng kiểu đối tượng lớp con bằng `std::make_unique`:
```cpp
// Factory Method: Khởi tạo đúng lớp con dựa vào cột 'loai' trong CSV
if (loai == "PHA_CHE")  return std::make_unique<PhaChe>(...);
if (loai == "THU_NGAN") return std::make_unique<ThuNgan>(...);
if (loai == "QUAN_LY")  return std::make_unique<QuanLy>(...);
if (loai == "PHU_VU")   return std::make_unique<PhuVu>(...);
if (loai == "BAO_VE")   return std::make_unique<BaoVe>(...);
```

---

## 4. THIẾT KẾ CƠ SỞ DỮ LIỆU & LƯU TRỮ TỆP
Dự án áp dụng mô hình **Flat-file Database kết hợp In-Memory Cache**. Dữ liệu được tổ chức dưới dạng các tệp CSV chuẩn hóa trong thư mục `data/`:

| Tên tệp | Mục đích lưu trữ | Khóa chính / Khóa ngoại |
|---|---|---|
| `nhanvien.csv` | Thông tin hồ sơ, chức vụ, lương cơ bản, avatar | Khóa chính: `maNV` |
| `taikhoan.csv` | Tên đăng nhập, mật khẩu, vai trò tài khoản | Khóa ngoại: `maNV` liên kết `nhanvien.csv` |
| `lichlamviec.csv` | Lịch các ca làm việc, giờ bắt đầu/kết thúc | Khóa ngoại: `dsMaNV` (danh sách mã NV) |
| `chamcong.csv` | Nhật ký check-in, check-out thời gian thực | Khóa ngoại: `maNV` |
| `luong.csv` | Lịch sử bảng lương đã kết toán theo tháng | Khóa ngoại: `maNV` |
| `menu.csv` | Danh mục thực đơn đồ uống, đơn giá, đường dẫn hình ảnh | Khóa chính: `maMon` |
| `ban_order.csv` | Trạng thái 30 bàn và danh sách món chi tiết đang order | Khóa chính: `soBan, maMon` |
| `hoadon.csv` | Lịch sử hóa đơn thanh toán phục vụ thống kê doanh thu | Khóa chính: `maHD` |
| `ketca.csv` | Dữ liệu tiền đầu ca, doanh thu ca, tổng kết ca và người chốt ca | Khóa chính: `maKetCa` (ngay_maCa) |

---

## 5. CẤU TRÚC THƯ MỤC DỰ ÁN
```
quanlycafe/
├── data/                  # Cơ sở dữ liệu tệp CSV (nhanvien, lichlamviec, menu, ban_order, hoadon,...)
│   ├── avatars/           # Thư mục lưu trữ hình ảnh đại diện của nhân viên
│   └── menu/              # Thư mục lưu trữ hình ảnh thức uống trong menu
├── gui/                   # Mã nguồn giao diện đồ họa Qt6 (MainWindow, Dialogs, AvatarHelper)
├── include/               # Header files (.h) định nghĩa Model, Controller, Order, KDS
├── src/                   # Source files (.cpp) hiện thực logic nghiệp vụ, bàn ăn, thực đơn
├── Build_GUI.bat          # Script tự động biên dịch dự án
├── Chay_App_GUI.bat       # Script khởi chạy ứng dụng 1-click
├── Day_Len_Github.bat     # Script tự động đẩy mã nguồn lên GitHub
├── CMakeLists.txt         # Cấu hình biên dịch CMake cho ứng dụng Qt6
└── Makefile               # Cấu hình biên dịch Make cho phiên bản dòng lệnh (CLI)
```

---

## 6. HƯỚNG DẪN BIÊN DỊCH VÀ KHỞI CHẠY
- **Yêu cầu môi trường:** Hệ điều hành Windows 10/11 64-bit; môi trường MSYS2 UCRT64 với bộ công cụ GCC, CMake, Ninja và Qt6 Base.
- **Khởi chạy nhanh 1-Click:** Nhấp đúp chuột vào tệp `Chay_App_GUI.bat`. Script sẽ tự động kiểm tra, biên dịch nếu cần và khởi chạy ứng dụng.
- **Biên dịch lại toàn bộ:** Nhấp đúp chuột vào tệp `Build_GUI.bat` để biên dịch lại mã nguồn từ đầu.
- **Đồng bộ GitHub:** Nhấp đúp chuột vào tệp `Day_Len_Github.bat` để tự động đẩy mã nguồn lên kho chứa GitHub.

### Danh sách tài khoản đăng nhập kiểm thử (Đồng bộ chuẩn xác từ data/nhanvien.csv):
| Mã NV / Tài khoản | Mật khẩu | Vai trò | Họ và tên | Chức vụ (Vị trí) | Mức lương cơ sở | Phụ cấp |
|---|---|---|---|---|---|---|
| `admin` | `admin123` | ADMIN | Quản trị viên | Quản trị toàn hệ thống | — | — |
| `NV1` | `123456` | EMPLOYEE | Nguyễn Thọ Phát | Quản lý (QUAN_LY) | 10.000.000 đ/tháng | 1.000.000 đ |
| `NV2` | `123456` | EMPLOYEE | Phạm Hoàng Hải | Thu ngân (THU_NGAN) | 25.000 đ/giờ | 500.000 đ |
| `NV3` | `123456` | EMPLOYEE | Lê Thị Thanh Thảo | Pha chế (PHA_CHE) | 20.000 đ/giờ | — |
| `NV4` | `123456` | EMPLOYEE | Đinh Thành Quang | Bảo vệ (BAO_VE) | 5.500.000 đ/tháng | — |
| `NV5` | `123456` | EMPLOYEE | Nguyễn Văn Phú Thành | Phục vụ (PHU_VU) | 18.000 đ/giờ | — |
| `NV6` | `123456` | EMPLOYEE | Phan Phú Quân | Phục vụ (PHU_VU) | 18.000 đ/giờ | — |
| `NV7` | `123456` | EMPLOYEE | Lê Nguyễn Hoàng Việt | Pha chế (PHA_CHE) | 20.000 đ/giờ | — |
