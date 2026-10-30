#pragma once
#include <string>
#include <vector>

// Đại diện cho 1 hóa đơn thanh toán
struct HoaDon {
    std::string maHD;
    int soBan = 0;
    double tongTien = 0.0;
    std::string ngay;     // "dd/MM/yyyy"
    std::string gio;      // "HH:mm"
    std::string nhanVien; // Mã NV hoặc Tên đăng nhập

    HoaDon() = default;
    HoaDon(std::string mhd, int sb, double tt, std::string ng, std::string g, std::string nv)
        : maHD(std::move(mhd)), soBan(sb), tongTien(tt), ngay(std::move(ng)), gio(std::move(g)), nhanVien(std::move(nv)) {}
};

// Đại diện cho thông tin kết ca của 1 ca làm việc
struct KetCa {
    std::string maKetCa;       // Khóa chính: ngay_maCa (VD: 30/10/2026_CA_SANG)
    std::string ngay;          // "dd/MM/yyyy"
    std::string maCa;          // "CA_SANG", "CA_CHIEU", "CA_TOI",...
    std::string tenCa;         // "Ca Sang",...
    double tienDauCa = 0.0;    // Số tiền bắt đầu ca
    double doanhThuCa = 0.0;   // Tổng tiền các hóa đơn thu được trong ca
    double tongTienKetCa = 0.0; // tienDauCa + doanhThuCa (hoặc tiền thực tế kiểm đếm)
    std::string nguoiChot;     // Người chốt ca (admin hoặc thu ngân)
    std::string thoiGianChot;  // "HH:mm dd/MM/yyyy"
    std::string ghiChu;

    KetCa() = default;
};

// Tổng kết doanh thu ngày
struct DoanhThuNgay {
    std::string ngay;
    double tongTienDauCa = 0.0;
    double tongDoanhThu = 0.0;
    double tongTienCuoiNgay = 0.0; // tongTienDauCa + tongDoanhThu
    int soHoaDon = 0;
    int soCaDaKet = 0;
};

// Tổng kết doanh thu tháng
struct DoanhThuThang {
    std::string thangNam; // "MM/yyyy"
    double tongDoanhThu = 0.0;
    int tongSoHoaDon = 0;
    std::vector<DoanhThuNgay> chiTietCacNgay;
};
