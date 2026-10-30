#include "BanAn.h"
#include <algorithm>

BanAn::BanAn(int so)
    : soBan(so), coKhach(false), thoiGianVao("") {}

int BanAn::getSoBan() const {
    return soBan;
}

bool BanAn::coNguoi() const {
    return coKhach;
}

void BanAn::setCoNguoi(bool cn) {
    coKhach = cn;
}

std::string BanAn::getThoiGianVao() const {
    return thoiGianVao;
}

void BanAn::setThoiGianVao(const std::string& tg) {
    thoiGianVao = tg;
}

const std::vector<ChiTietOrder>& BanAn::getDsMon() const {
    return dsMon;
}

std::vector<ChiTietOrder>& BanAn::getDsMonRef() {
    return dsMon;
}

void BanAn::themMon(const MonAn& mon, int soLuong, const std::string& thoiGian) {
    if (soLuong <= 0) return;
    coKhach = true;
    if (thoiGianVao.empty()) {
        thoiGianVao = thoiGian;
    }

    // Nếu món đã có trong bàn và chưa bưng xong, cộng dồn số lượng
    for (auto& item : dsMon) {
        if (item.maMon == mon.maMon) {
            item.soLuong += soLuong;
            // Nếu trước đó đã làm xong mà gọi thêm, chuyển lại về chờ làm
            if (item.trangThai == TrangThaiMonOrder::DA_XONG || item.trangThai == TrangThaiMonOrder::DA_PHUC_VU) {
                item.trangThai = TrangThaiMonOrder::CHO_LAM;
            }
            return;
        }
    }

    // Nếu chưa có món, thêm mới
    dsMon.emplace_back(mon.maMon, mon.tenMon, mon.donGia, soLuong, thoiGian, TrangThaiMonOrder::CHO_LAM);
}

bool BanAn::suaSoLuong(const std::string& maMon, int soLuongMoi) {
    if (soLuongMoi <= 0) {
        return xoaMon(maMon);
    }
    for (auto& item : dsMon) {
        if (item.maMon == maMon) {
            item.soLuong = soLuongMoi;
            return true;
        }
    }
    return false;
}

bool BanAn::xoaMon(const std::string& maMon) {
    auto it = std::remove_if(dsMon.begin(), dsMon.end(),
        [&maMon](const ChiTietOrder& item) { return item.maMon == maMon; });
    if (it != dsMon.end()) {
        dsMon.erase(it, dsMon.end());
        if (dsMon.empty()) {
            coKhach = false;
            thoiGianVao = "";
        }
        return true;
    }
    return false;
}

bool BanAn::datTrangThaiMon(const std::string& maMon, TrangThaiMonOrder tt) {
    for (auto& item : dsMon) {
        if (item.maMon == maMon) {
            item.trangThai = tt;
            return true;
        }
    }
    return false;
}

double BanAn::tinhTongTien() const {
    double tong = 0.0;
    for (const auto& item : dsMon) {
        tong += item.tinhThanhTien();
    }
    return tong;
}

int BanAn::tongSoLuongMon() const {
    int tong = 0;
    for (const auto& item : dsMon) {
        tong += item.soLuong;
    }
    return tong;
}

void BanAn::resetBan() {
    dsMon.clear();
    coKhach = false;
    thoiGianVao = "";
}
