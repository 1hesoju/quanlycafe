#pragma once
#include <string>
#include <vector>
#include <map>
#include "DoanhThu.h"
#include "CaLamViec.h"

class QuanLyDoanhThu {
private:
    std::vector<HoaDon> dsHoaDon;
    std::map<std::string, KetCa> dsKetCa; // key: ngay_maCa (VD: "30/10/2026_CA_SANG")

public:
    QuanLyDoanhThu() = default;

    // Quản lý hóa đơn
    void themHoaDon(const HoaDon& hd);
    const std::vector<HoaDon>& layDsHoaDon() const;
    std::vector<HoaDon> layHoaDonTheoNgay(const std::string& ngay) const;
    std::vector<HoaDon> layHoaDonTrongCa(const std::string& ngay, const std::string& gioBatDau, const std::string& gioKetThuc) const;

    // Quản lý kết ca & tiền đầu ca
    bool datTienDauCa(const std::string& ngay, const std::string& maCa, const std::string& tenCa, double tienDauCa);
    bool ketCaLamViec(const std::string& ngay, const std::string& maCa, const std::string& tenCa,
                      double tienDauCa, double doanhThuCa, double tongTienKetCa,
                      const std::string& nguoiChot, const std::string& ghiChu = "");
    KetCa* timKetCa(const std::string& ngay, const std::string& maCa);
    const KetCa* timKetCa(const std::string& ngay, const std::string& maCa) const;
    std::vector<KetCa> layDsKetCaTheoNgay(const std::string& ngay) const;

    // Thống kê doanh thu ngày & tháng
    DoanhThuNgay thongKeDoanhThuNgay(const std::string& ngay) const;
    DoanhThuThang thongKeDoanhThuThang(const std::string& thangNam) const;

    // Lưu & tải file
    bool taiHoaDon(const std::string& filePath);
    bool luuHoaDon(const std::string& filePath) const;
    bool taiKetCa(const std::string& filePath);
    bool luuKetCa(const std::string& filePath) const;

    void taiTatCa(const std::string& thuMuc);
    void luuTatCa(const std::string& thuMuc) const;
};
