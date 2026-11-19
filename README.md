# He Thong Quan Ly Quan Cafe (Coffee Shop Management System)

He thong quan ly nhan su, ca lam viec, cham cong, tinh luong, goi mon iPOS 30 ban, man hinh pha che KDS, phuc vu va ket ca doanh thu cho quan ca phe, duoc phat trien bang **C++ (C++17)** voi kien truc chuan **MVC**, ho tro ca hai giao dien: **Console CLI** va **Desktop GUI bang Qt6**.

---

## 🐧 Huong Dan Cai Dat & Chay Tren Linux (Ubuntu / Debian / Arch / Fedora)

Dự án tương thích hoàn toàn 100% với hệ điều hành Linux.

### 1. Cài đặt các gói thư viện cần thiết

* **Trên Ubuntu / Debian / Linux Mint:**
  ```bash
  sudo apt update
  sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools libgl1-mesa-dev
  ```

* **Trên Arch Linux / Manjaro:**
  ```bash
  sudo pacman -S --needed base-devel cmake qt6-base
  ```

* **Trên Fedora / RHEL:**
  ```bash
  sudo dnf install -y gcc-c++ cmake qt6-qtbase-devel
  ```

### 2. Biên dịch & Khởi chạy ứng dụng GUI trên Linux

Chạy script tự động (đã được cấp quyền thực thi):
```bash
# Cấp quyền thực thi nếu cần
chmod +x Build_GUI.sh Chay_App_GUI.sh

# Biên dịch ứng dụng
./Build_GUI.sh

# Khởi chạy ứng dụng
./Chay_App_GUI.sh
# hoặc: ./quanlycafe_gui
```

### 3. Hoặc biên dịch bản Console CLI (không cần Qt6):
```bash
make
./quanlycafe
```

---

## ⚡ Huong Dan Tren Windows (MSYS2 UCRT64)

### Buoc 1: Cai dat moi truong (Neu may chua co MSYS2 UCRT64 va Qt6)
1. Cai dat [MSYS2](https://www.msys2.org/) (mac dinh tai `C:\msys64`).
2. Mo **MSYS2 UCRT64** (tim trong Menu Start) va chay duy nhat 1 dong lenh sau:
   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-qt6-base
   ```

### Buoc 2: Bien dich ra file `.exe`
- Nhap dup chuot vao file **`Build_GUI.bat`** (hoac nhap dup **`Chay_App_GUI.bat`**).
- Ung dung se tu dong sinh ra file `quanlycafe_gui.exe` va khoi chay ngay lap tuc!

---

## Cac Tinh Nang Chinh

### 1. Quan Ly Nhan Vien
- Them, sua, cap nhat thong tin nhan vien (Full-time & Part-time).
- Ho tro quan ly trang thai nhan vien (Dang lam viec, Tam nghi, Da nghi viec).
- Tich hop Avatar tron tu dong va avatar chu cai viet tat thong minh.
- Bo loc & tim kiem da tieu chi (theo ten, ma, vi tri, trang thai, muc luong).

### 2. Quan Ly Lich Lam Viec & Chia Ca
- **3 ca lam viec co dinh moi ngay:**
  - Ca Sang: `07:00 - 12:00`
  - Ca Chieu: `12:00 - 17:00`
  - Ca Toi: `17:00 - 22:00`
- Cho phep 1 nhan vien lam nhieu ca trong ngay neu khong trung gio thuc te.
- Thuat toan kiem tra chong trung ca lam viec va chuan hoa lich khi luu.

### 3. Cham Cong & Tinh Luong
- Cham cong thong minh (Smart Timekeeping): Check-in som toi da 30 phut, phan loai Dung gio, Di muon, Ve som.
- Bang luong tu dong theo thang cho nhan vien Full-time (luong thang + phu cap) va Part-time (theo so gio lam).

### 4. Goi Mon iPOS & So Do 30 Ban
- So do 30 ban ngoi truc quan: Ban co khach in mau Xanh nuoc bien, ban trong mau Xam.
- Menu 8 mon thuc uong voi anh truc quan, gia tien ro rang.
- Chot order, sua/xoa mon va thanh toan xuat hoa don ve ban trong (mau xam).

### 5. Man Hinh Pha Che (KDS) & Thong Bao Phuc Vu
- Hang doi mon goi theo thoi gian thuc (FIFO) cho quay Pha Che (`PHA_CHE`).
- Nut Xac nhan (Dang lam) va Hoan thanh (Da xong).
- Tu dong ban thong bao Popup toi nhan vien Phuc Vu (`PHU_VU`) de mang mon ra ban.

### 6. Ket Ca Theo Ngay & Doanh Thu Thang
- Phan quyen nghiem ngat chi danh cho `Admin` va `Thu Ngan` (`THU_NGAN`).
- Nhap so tien bat dau ca, tu dong bop tach doanh thu tung ca va chot ket ca.
- Tong hop doanh thu ban hang va so luong don hang thuc te trong thang.

---

## Cau Truc Thu Muc

```
quanlycafe/
├── data/                  # Du lieu CSV (nhanvien, lichlamviec, menu, ban_order, hoadon, ketca...)
│   ├── avatars/           # Anh dai dien nhan vien
│   └── menu/              # Anh thuc uong trong menu
├── include/               # Header files (.h) kien truc Model & Controller
├── src/                   # Source code C++ (.cpp) cho Logic nghiep vu
├── gui/                   # Ma nguon giao dien do hoa Qt6 (MainWindow, Dialogs...)
├── CMakeLists.txt         # Cau hinh build CMake cho GUI Qt6 (Ho tro Windows & Linux)
├── Makefile               # Makefile cho phien ban Console CLI
├── Build_GUI.sh           # Script build tu dong tren Linux
├── Chay_App_GUI.sh        # Script chay app 1-click tren Linux
├── Build_GUI.bat          # Script build tu dong tren Windows
├── Chay_App_GUI.bat       # Script chay app 1-click tren Windows
└── README.md
```

---

## Tai Khoan Kiem Thu Mac Dinh

- **Quan tri vien (Admin):** `admin` / `admin123`
- **Nhan vien:**
  - `NV1` (Quan ly): `123456`
  - `NV2` (Thu ngan): `123456`
  - `NV3` (Pha che): `123456`
  - `NV4` (Bao ve): `123456`
  - `NV5` (Phuc vu): `123456`
  - `NV6` (Phuc vu): `123456`
  - `NV7` (Pha che): `123456`
