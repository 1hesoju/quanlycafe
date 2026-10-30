#pragma once
#include <string>
#include <vector>
#include "MonAn.h"
#include "BanAn.h"

struct ItemPhaChe {
    int soBan;
    ChiTietOrder chiTiet;
};

class QuanLyOrder {
private:
    std::vector<MonAn> dsMenu;
    std::vector<BanAn> dsBan; // Luôn có 30 bàn (chỉ số 0..29 tương ứng bàn 1..30)
    std::vector<std::string> dsThongBaoPhuVu;

public:
    QuanLyOrder();

    void khoiTaoMacDinh();

    // Quản lý thực đơn (Menu)
    const std::vector<MonAn>& layMenu() const;
    const MonAn* timMon(const std::string& maMon) const;
    bool taiMenu(const std::string& filePath);
    bool luuMenu(const std::string& filePath) const;

    // Quản lý bàn
    BanAn* layBan(int soBan);
    const BanAn* layBan(int soBan) const;
    const std::vector<BanAn>& layDanhSachBan() const;

    // Thao tác Order
    bool datMon(int soBan, const std::string& maMon, int soLuong, const std::string& thoiGian);
    bool suaMon(int soBan, const std::string& maMon, int soLuongMoi);
    bool xoaMon(int soBan, const std::string& maMon);
    bool thanhToan(int soBan, const std::string& nhanVien, const std::string& duongDanHoaDon = "data/hoadon.csv");

    // Nghiệp vụ Pha Chế (KDS)
    std::vector<ItemPhaChe> layDanhSachCanPhaChe() const;
    bool xacNhanPhaChe(int soBan, const std::string& maMon);
    bool hoanThanhPhaChe(int soBan, const std::string& maMon);

    // Nghiệp vụ Phục Vụ
    std::vector<ItemPhaChe> layDanhSachCanBung() const;
    bool xacNhanDaBung(int soBan, const std::string& maMon);
    std::vector<std::string> layVaXoaThongBaoPhuVu();
    bool coThongBaoMoi() const;

    // Lưu & Tải trạng thái bàn
    bool taiTrangThaiBan(const std::string& filePath);
    bool luuTrangThaiBan(const std::string& filePath) const;

    void taiTatCa(const std::string& thuMuc);
    void luuTatCa(const std::string& thuMuc) const;
};
