#include "MonAn.h"

std::string trangThaiMonToString(TrangThaiMonOrder tt) {
    switch (tt) {
        case TrangThaiMonOrder::CHO_LAM: return "CHO_LAM";
        case TrangThaiMonOrder::DANG_LAM: return "DANG_LAM";
        case TrangThaiMonOrder::DA_XONG: return "DA_XONG";
        case TrangThaiMonOrder::DA_PHUC_VU: return "DA_PHUC_VU";
    }
    return "CHO_LAM";
}

TrangThaiMonOrder stringToTrangThaiMon(const std::string& str) {
    if (str == "DANG_LAM") return TrangThaiMonOrder::DANG_LAM;
    if (str == "DA_XONG") return TrangThaiMonOrder::DA_XONG;
    if (str == "DA_PHUC_VU") return TrangThaiMonOrder::DA_PHUC_VU;
    return TrangThaiMonOrder::CHO_LAM;
}
