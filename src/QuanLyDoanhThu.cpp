#include "QuanLyDoanhThu.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>

void QuanLyDoanhThu::themHoaDon(const HoaDon& hd) {
    dsHoaDon.push_back(hd);
}

const std::vector<HoaDon>& QuanLyDoanhThu::layDsHoaDon() const {
    return dsHoaDon;
}

std::vector<HoaDon> QuanLyDoanhThu::layHoaDonTheoNgay(const std::string& ngay) const {
    std::vector<HoaDon> res;
    for (const auto& hd : dsHoaDon) {
        if (hd.ngay == ngay) {
            res.push_back(hd);
        }
    }
    return res;
}

std::vector<HoaDon> QuanLyDoanhThu::layHoaDonTrongCa(const std::string& ngay, const std::string& gioBatDau, const std::string& gioKetThuc) const {
    std::vector<HoaDon> res;
    int pBatDau = Utils::gioToPhut(gioBatDau);
    int pKetThuc = Utils::gioToPhut(gioKetThuc);

    for (const auto& hd : dsHoaDon) {
        if (hd.ngay != ngay) continue;
        int pHD = Utils::gioToPhut(hd.gio);
        if (pHD < 0) continue;

        if (pBatDau <= pKetThuc) {
            // Ca bình thường trong ngày
            if (pHD >= pBatDau && pHD < pKetThuc) {
                res.push_back(hd);
            }
        } else {
            // Ca vắt qua đêm (VD 22:00 -> 06:00)
            if (pHD >= pBatDau || pHD < pKetThuc) {
                res.push_back(hd);
            }
        }
    }
    return res;
}

bool QuanLyDoanhThu::datTienDauCa(const std::string& ngay, const std::string& maCa, const std::string& tenCa, double tienDauCa) {
    std::string key = ngay + "_" + maCa;
    KetCa& kc = dsKetCa[key];
    kc.maKetCa = key;
    kc.ngay = ngay;
    kc.maCa = maCa;
    kc.tenCa = tenCa;
    kc.tienDauCa = tienDauCa;
    return true;
}

bool QuanLyDoanhThu::ketCaLamViec(const std::string& ngay, const std::string& maCa, const std::string& tenCa,
                                  double tienDauCa, double doanhThuCa, double tongTienKetCa,
                                  const std::string& nguoiChot, const std::string& ghiChu) {
    std::string key = ngay + "_" + maCa;
    KetCa& kc = dsKetCa[key];
    kc.maKetCa = key;
    kc.ngay = ngay;
    kc.maCa = maCa;
    kc.tenCa = tenCa;
    kc.tienDauCa = tienDauCa;
    kc.doanhThuCa = doanhThuCa;
    kc.tongTienKetCa = tongTienKetCa;
    kc.nguoiChot = nguoiChot;
    kc.thoiGianChot = Utils::gioHienTai() + " " + Utils::ngayHomNay();
    kc.ghiChu = ghiChu;
    return true;
}

KetCa* QuanLyDoanhThu::timKetCa(const std::string& ngay, const std::string& maCa) {
    std::string key = ngay + "_" + maCa;
    auto it = dsKetCa.find(key);
    if (it != dsKetCa.end()) return &it->second;
    return nullptr;
}

const KetCa* QuanLyDoanhThu::timKetCa(const std::string& ngay, const std::string& maCa) const {
    std::string key = ngay + "_" + maCa;
    auto it = dsKetCa.find(key);
    if (it != dsKetCa.end()) return &it->second;
    return nullptr;
}

std::vector<KetCa> QuanLyDoanhThu::layDsKetCaTheoNgay(const std::string& ngay) const {
    std::vector<KetCa> res;
    for (const auto& p : dsKetCa) {
        if (p.second.ngay == ngay) {
            res.push_back(p.second);
        }
    }
    return res;
}

DoanhThuNgay QuanLyDoanhThu::thongKeDoanhThuNgay(const std::string& ngay) const {
    DoanhThuNgay dt;
    dt.ngay = ngay;

    for (const auto& hd : dsHoaDon) {
        if (hd.ngay == ngay) {
            dt.tongDoanhThu += hd.tongTien;
            dt.soHoaDon++;
        }
    }

    for (const auto& p : dsKetCa) {
        if (p.second.ngay == ngay) {
            dt.tongTienDauCa += p.second.tienDauCa;
            if (!p.second.nguoiChot.empty()) {
                dt.soCaDaKet++;
            }
        }
    }

    dt.tongTienCuoiNgay = dt.tongTienDauCa + dt.tongDoanhThu;
    return dt;
}

DoanhThuThang QuanLyDoanhThu::thongKeDoanhThuThang(const std::string& thangNam) const {
    DoanhThuThang res;
    res.thangNam = thangNam;

    // Lấy tập hợp các ngày có dữ liệu trong tháng
    std::set<std::string> tapHopNgay;
    for (const auto& hd : dsHoaDon) {
        if (Utils::layThangNamTuNgay(hd.ngay) == thangNam) {
            tapHopNgay.insert(hd.ngay);
        }
    }
    for (const auto& p : dsKetCa) {
        if (Utils::layThangNamTuNgay(p.second.ngay) == thangNam) {
            tapHopNgay.insert(p.second.ngay);
        }
    }

    for (const auto& ng : tapHopNgay) {
        DoanhThuNgay dtNg = thongKeDoanhThuNgay(ng);
        res.tongDoanhThu += dtNg.tongDoanhThu;
        res.tongSoHoaDon += dtNg.soHoaDon;
        res.chiTietCacNgay.push_back(dtNg);
    }

    // Sắp xếp ngày từ mới nhất đến cũ nhất
    std::sort(res.chiTietCacNgay.begin(), res.chiTietCacNgay.end(), [](const DoanhThuNgay& a, const DoanhThuNgay& b) {
        // So sánh theo ngày format dd/MM/yyyy
        auto pA = Utils::split(a.ngay, '/');
        auto pB = Utils::split(b.ngay, '/');
        if (pA.size() == 3 && pB.size() == 3) {
            if (pA[2] != pB[2]) return pA[2] > pB[2];
            if (pA[1] != pB[1]) return pA[1] > pB[1];
            return pA[0] > pB[0];
        }
        return a.ngay > b.ngay;
    });

    return res;
}

bool QuanLyDoanhThu::taiHoaDon(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    dsHoaDon.clear();
    std::string line;
    bool isHeader = true;

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            isHeader = false;
            if (line.find("maHD") != std::string::npos) continue;
        }

        auto tokens = Utils::splitAndTrim(line, ',');
        if (tokens.size() >= 5) {
            std::string maHD = tokens[0];
            int sb = 0;
            double tt = 0.0;
            try { sb = std::stoi(tokens[1]); } catch (...) {}
            try { tt = std::stod(tokens[2]); } catch (...) {}
            std::string thoiGian = tokens[3]; // "dd/MM/yyyy HH:mm"
            std::string nv = tokens[4];

            // Tách thoiGian thành ngay và gio
            std::string ngay, gio;
            auto tgParts = Utils::splitAndTrim(thoiGian, ' ');
            if (tgParts.size() >= 2) {
                // Có thể lưu dạng "dd/MM/yyyy HH:mm"
                if (tgParts[0].find('/') != std::string::npos) {
                    ngay = tgParts[0];
                    gio = tgParts[1];
                } else {
                    gio = tgParts[0];
                    ngay = tgParts[1];
                }
            } else if (!thoiGian.empty()) {
                ngay = thoiGian;
                gio = "00:00";
            }

            dsHoaDon.emplace_back(maHD, sb, tt, ngay, gio, nv);
        }
    }
    return true;
}

bool QuanLyDoanhThu::luuHoaDon(const std::string& filePath) const {
    std::ofstream f(filePath);
    if (!f.is_open()) return false;

    f << "maHD,soBan,tongTien,thoiGian,nhanVien\n";
    for (const auto& hd : dsHoaDon) {
        f << hd.maHD << "," << hd.soBan << "," << std::fixed << std::setprecision(0) << hd.tongTien
          << "," << hd.ngay << " " << hd.gio << "," << hd.nhanVien << "\n";
    }
    return true;
}

bool QuanLyDoanhThu::taiKetCa(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return false;

    dsKetCa.clear();
    std::string line;
    bool isHeader = true;

    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            isHeader = false;
            if (line.find("maKetCa") != std::string::npos) continue;
        }

        auto tokens = Utils::splitAndTrim(line, ',');
        if (tokens.size() >= 7) {
            KetCa kc;
            kc.maKetCa = tokens[0];
            kc.ngay = tokens[1];
            kc.maCa = tokens[2];
            kc.tenCa = tokens[3];
            try { kc.tienDauCa = std::stod(tokens[4]); } catch (...) {}
            try { kc.doanhThuCa = std::stod(tokens[5]); } catch (...) {}
            try { kc.tongTienKetCa = std::stod(tokens[6]); } catch (...) {}
            if (tokens.size() >= 8) kc.nguoiChot = tokens[7];
            if (tokens.size() >= 9) kc.thoiGianChot = tokens[8];
            if (tokens.size() >= 10) kc.ghiChu = tokens[9];

            dsKetCa[kc.maKetCa] = kc;
        }
    }
    return true;
}

bool QuanLyDoanhThu::luuKetCa(const std::string& filePath) const {
    std::ofstream f(filePath);
    if (!f.is_open()) return false;

    f << "maKetCa,ngay,maCa,tenCa,tienDauCa,doanhThuCa,tongTienKetCa,nguoiChot,thoiGianChot,ghiChu\n";
    for (const auto& p : dsKetCa) {
        const auto& kc = p.second;
        f << kc.maKetCa << "," << kc.ngay << "," << kc.maCa << "," << kc.tenCa << ","
          << std::fixed << std::setprecision(0) << kc.tienDauCa << ","
          << std::fixed << std::setprecision(0) << kc.doanhThuCa << ","
          << std::fixed << std::setprecision(0) << kc.tongTienKetCa << ","
          << kc.nguoiChot << "," << kc.thoiGianChot << "," << kc.ghiChu << "\n";
    }
    return true;
}

void QuanLyDoanhThu::taiTatCa(const std::string& thuMuc) {
    taiHoaDon(thuMuc + "/hoadon.csv");
    taiKetCa(thuMuc + "/ketca.csv");
}

void QuanLyDoanhThu::luuTatCa(const std::string& thuMuc) const {
    luuHoaDon(thuMuc + "/hoadon.csv");
    luuKetCa(thuMuc + "/ketca.csv");
}
