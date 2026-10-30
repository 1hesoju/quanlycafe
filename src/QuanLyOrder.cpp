#include "QuanLyOrder.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>

QuanLyOrder::QuanLyOrder() {
    khoiTaoMacDinh();
}

void QuanLyOrder::khoiTaoMacDinh() {
    dsBan.clear();
    for (int i = 1; i <= 30; ++i) {
        dsBan.emplace_back(i);
    }

    dsMenu = {
        {"M01", "Ca Phe Muoi", 20000, "data/menu/caphe_muoi.png"},
        {"M02", "Bac Xiu Kem Muoi", 25000, "data/menu/bacxiu_kemmuoi.png"},
        {"M03", "Nau Da", 18000, "data/menu/nau_da.png"},
        {"M04", "Sua Tuoi Ca Phe", 20000, "data/menu/suatuoi_caphe.png"},
        {"M05", "Bac Xiu", 20000, "data/menu/bac_xiu.png"},
        {"M06", "Den Da", 15000, "data/menu/den_da.png"},
        {"M07", "Coldbrew", 20000, "data/menu/coldbrew.png"},
        {"M08", "Tra Tac Xi Muoi", 20000, "data/menu/tra_tac_ximuoi.png"}
    };
}

const std::vector<MonAn>& QuanLyOrder::layMenu() const {
    return dsMenu;
}

const MonAn* QuanLyOrder::timMon(const std::string& maMon) const {
    for (const auto& m : dsMenu) {
        if (m.maMon == maMon) return &m;
    }
    return nullptr;
}

bool QuanLyOrder::taiMenu(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    dsMenu.clear();
    std::string line;
    bool isHeader = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            isHeader = false;
            if (line.find("maMon") != std::string::npos) continue;
        }
        auto tokens = Utils::splitAndTrim(line, ',');
        if (tokens.size() >= 4) {
            double gia = 0.0;
            try { gia = std::stod(tokens[2]); } catch (...) { gia = 0.0; }
            dsMenu.emplace_back(tokens[0], tokens[1], gia, tokens[3]);
        }
    }
    return !dsMenu.empty();
}

bool QuanLyOrder::luuMenu(const std::string& filePath) const {
    std::ofstream f(filePath);
    if (!f.is_open()) return false;
    f << "maMon,tenMon,donGia,hinhAnh\n";
    for (const auto& m : dsMenu) {
        f << m.maMon << "," << m.tenMon << "," << std::fixed << std::setprecision(0) << m.donGia << "," << m.hinhAnh << "\n";
    }
    return true;
}

BanAn* QuanLyOrder::layBan(int soBan) {
    if (soBan >= 1 && soBan <= static_cast<int>(dsBan.size())) {
        return &dsBan[soBan - 1];
    }
    return nullptr;
}

const BanAn* QuanLyOrder::layBan(int soBan) const {
    if (soBan >= 1 && soBan <= static_cast<int>(dsBan.size())) {
        return &dsBan[soBan - 1];
    }
    return nullptr;
}

const std::vector<BanAn>& QuanLyOrder::layDanhSachBan() const {
    return dsBan;
}

bool QuanLyOrder::datMon(int soBan, const std::string& maMon, int soLuong, const std::string& thoiGian) {
    BanAn* b = layBan(soBan);
    if (!b) return false;
    const MonAn* mon = timMon(maMon);
    if (!mon) return false;

    std::string tg = thoiGian.empty() ? Utils::gioHienTai() : thoiGian;
    b->themMon(*mon, soLuong, tg);
    return true;
}

bool QuanLyOrder::suaMon(int soBan, const std::string& maMon, int soLuongMoi) {
    BanAn* b = layBan(soBan);
    if (!b) return false;
    return b->suaSoLuong(maMon, soLuongMoi);
}

bool QuanLyOrder::xoaMon(int soBan, const std::string& maMon) {
    BanAn* b = layBan(soBan);
    if (!b) return false;
    return b->xoaMon(maMon);
}

bool QuanLyOrder::thanhToan(int soBan, const std::string& nhanVien, const std::string& duongDanHoaDon) {
    BanAn* b = layBan(soBan);
    if (!b || !b->coNguoi()) return false;

    double tongTien = b->tinhTongTien();
    std::string ngayGio = Utils::ngayHomNay() + " " + Utils::gioHienTai();

    // Lưu hóa đơn vào file lịch sử
    std::ofstream f(duongDanHoaDon, std::ios::app);
    if (f.is_open()) {
        static bool headerChecked = false;
        if (!headerChecked) {
            std::ifstream check(duongDanHoaDon);
            if (check.peek() == std::ifstream::traits_type::eof()) {
                f << "maHD,soBan,tongTien,thoiGian,nhanVien\n";
            }
            headerChecked = true;
        }
        std::string maHD = "HD" + std::to_string(soBan) + "_" + std::to_string(std::time(nullptr));
        f << maHD << "," << soBan << "," << std::fixed << std::setprecision(0) << tongTien
          << "," << ngayGio << "," << nhanVien << "\n";
    }

    // Reset bàn về trạng thái trống (màu xám)
    b->resetBan();
    return true;
}

std::vector<ItemPhaChe> QuanLyOrder::layDanhSachCanPhaChe() const {
    std::vector<ItemPhaChe> result;
    for (const auto& b : dsBan) {
        if (!b.coNguoi()) continue;
        for (const auto& item : b.getDsMon()) {
            if (item.trangThai == TrangThaiMonOrder::CHO_LAM ||
                item.trangThai == TrangThaiMonOrder::DANG_LAM) {
                result.push_back({b.getSoBan(), item});
            }
        }
    }
    return result;
}

bool QuanLyOrder::xacNhanPhaChe(int soBan, const std::string& maMon) {
    BanAn* b = layBan(soBan);
    if (!b) return false;
    return b->datTrangThaiMon(maMon, TrangThaiMonOrder::DANG_LAM);
}

bool QuanLyOrder::hoanThanhPhaChe(int soBan, const std::string& maMon) {
    BanAn* b = layBan(soBan);
    if (!b) return false;

    for (auto& item : b->getDsMonRef()) {
        if (item.maMon == maMon) {
            item.trangThai = TrangThaiMonOrder::DA_XONG;
            std::string tb = "Ban " + std::to_string(soBan) + ": Da pha che xong " +
                             item.tenMon + " (SL: " + std::to_string(item.soLuong) + ") - Vui long bung ra ban!";
            dsThongBaoPhuVu.push_back(tb);
            return true;
        }
    }
    return false;
}

std::vector<ItemPhaChe> QuanLyOrder::layDanhSachCanBung() const {
    std::vector<ItemPhaChe> result;
    for (const auto& b : dsBan) {
        if (!b.coNguoi()) continue;
        for (const auto& item : b.getDsMon()) {
            if (item.trangThai == TrangThaiMonOrder::DA_XONG) {
                result.push_back({b.getSoBan(), item});
            }
        }
    }
    return result;
}

bool QuanLyOrder::xacNhanDaBung(int soBan, const std::string& maMon) {
    BanAn* b = layBan(soBan);
    if (!b) return false;
    return b->datTrangThaiMon(maMon, TrangThaiMonOrder::DA_PHUC_VU);
}

std::vector<std::string> QuanLyOrder::layVaXoaThongBaoPhuVu() {
    auto tmp = dsThongBaoPhuVu;
    dsThongBaoPhuVu.clear();
    return tmp;
}

bool QuanLyOrder::coThongBaoMoi() const {
    return !dsThongBaoPhuVu.empty();
}

bool QuanLyOrder::taiTrangThaiBan(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    // Reset 30 bàn
    for (auto& b : dsBan) {
        b.resetBan();
    }

    std::string line;
    bool isHeader = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            isHeader = false;
            if (line.find("soBan") != std::string::npos) continue;
        }
        auto tokens = Utils::splitAndTrim(line, ',');
        if (tokens.size() >= 3) {
            int sb = 0;
            try { sb = std::stoi(tokens[0]); } catch (...) { continue; }
            BanAn* b = layBan(sb);
            if (!b) continue;

            bool cn = (tokens[1] == "1" || tokens[1] == "true");
            b->setCoNguoi(cn);
            b->setThoiGianVao(tokens[2]);

            if (tokens.size() >= 8 && !tokens[3].empty()) {
                std::string maMon = tokens[3];
                std::string tenMon = tokens[4];
                double gia = 0.0;
                int sl = 1;
                try { gia = std::stod(tokens[5]); } catch (...) {}
                try { sl = std::stoi(tokens[6]); } catch (...) {}
                std::string tgDat = tokens[7];
                TrangThaiMonOrder tt = (tokens.size() >= 9) ? stringToTrangThaiMon(tokens[8]) : TrangThaiMonOrder::CHO_LAM;
                b->getDsMonRef().emplace_back(maMon, tenMon, gia, sl, tgDat, tt);
            }
        }
    }
    return true;
}

bool QuanLyOrder::luuTrangThaiBan(const std::string& filePath) const {
    std::ofstream f(filePath);
    if (!f.is_open()) return false;

    f << "soBan,coKhach,thoiGianVao,maMon,tenMon,donGia,soLuong,thoiGianDat,trangThai\n";
    for (const auto& b : dsBan) {
        if (!b.coNguoi() || b.getDsMon().empty()) {
            f << b.getSoBan() << "," << (b.coNguoi() ? "1" : "0") << "," << b.getThoiGianVao() << ",,,,,\n";
        } else {
            for (const auto& m : b.getDsMon()) {
                f << b.getSoBan() << ",1," << b.getThoiGianVao() << ","
                  << m.maMon << "," << m.tenMon << "," << std::fixed << std::setprecision(0) << m.donGia << ","
                  << m.soLuong << "," << m.thoiGianDat << "," << trangThaiMonToString(m.trangThai) << "\n";
            }
        }
    }
    return true;
}

void QuanLyOrder::taiTatCa(const std::string& thuMuc) {
    taiMenu(thuMuc + "/menu.csv");
    taiTrangThaiBan(thuMuc + "/ban_order.csv");
}

void QuanLyOrder::luuTatCa(const std::string& thuMuc) const {
    luuMenu(thuMuc + "/menu.csv");
    luuTrangThaiBan(thuMuc + "/ban_order.csv");
}
