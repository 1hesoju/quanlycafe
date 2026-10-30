#pragma once
#include <string>

// Trạng thái pha chế của từng món trong order
enum class TrangThaiMonOrder {
    CHO_LAM,      // Mới đặt, chờ quầy pha chế nhận
    DANG_LAM,     // Pha chế đã nhấn "Xác nhận", đang làm món
    DA_XONG,      // Pha chế đã nhấn "Hoàn thành", thông báo cho Phục vụ bưng
    DA_PHUC_VU    // Phục vụ đã bưng ra bàn cho khách
};

std::string trangThaiMonToString(TrangThaiMonOrder tt);
TrangThaiMonOrder stringToTrangThaiMon(const std::string& str);

// Thông tin món ăn trong Menu
struct MonAn {
    std::string maMon;
    std::string tenMon;
    double donGia;
    std::string hinhAnh; // đường dẫn tương đối, ví dụ "data/menu/caphe_muoi.png"

    MonAn(std::string ma = "", std::string ten = "", double gia = 0.0, std::string anh = "")
        : maMon(std::move(ma)), tenMon(std::move(ten)), donGia(gia), hinhAnh(std::move(anh)) {}
};

// Chi tiết 1 món đã gọi trong order của bàn
struct ChiTietOrder {
    std::string maMon;
    std::string tenMon;
    double donGia;
    int soLuong;
    std::string thoiGianDat; // "HH:mm:ss"
    TrangThaiMonOrder trangThai;

    ChiTietOrder(std::string ma = "", std::string ten = "", double gia = 0.0, int sl = 1,
                 std::string tg = "", TrangThaiMonOrder tt = TrangThaiMonOrder::CHO_LAM)
        : maMon(std::move(ma)), tenMon(std::move(ten)), donGia(gia), soLuong(sl),
          thoiGianDat(std::move(tg)), trangThai(tt) {}

    double tinhThanhTien() const {
        return donGia * soLuong;
    }
};
