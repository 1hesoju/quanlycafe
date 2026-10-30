#pragma once
#include <string>
#include <vector>
#include "MonAn.h"

class BanAn {
private:
    int soBan;
    bool coKhach;
    std::string thoiGianVao;
    std::vector<ChiTietOrder> dsMon;

public:
    explicit BanAn(int so = 1);

    int getSoBan() const;
    bool coNguoi() const;
    void setCoNguoi(bool cn);
    std::string getThoiGianVao() const;
    void setThoiGianVao(const std::string& tg);

    const std::vector<ChiTietOrder>& getDsMon() const;
    std::vector<ChiTietOrder>& getDsMonRef();

    void themMon(const MonAn& mon, int soLuong, const std::string& thoiGian);
    bool suaSoLuong(const std::string& maMon, int soLuongMoi);
    bool xoaMon(const std::string& maMon);
    bool datTrangThaiMon(const std::string& maMon, TrangThaiMonOrder tt);

    double tinhTongTien() const;
    int tongSoLuongMon() const;
    void resetBan(); // Trả bàn trống sau khi thanh toán
};
