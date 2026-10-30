#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTimer>
#include "QuanLyQuanCafe.h"
#include "TaiKhoan.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QuanLyQuanCafe& app;
    QuanLyTaiKhoan& qlTK;
    std::string thuMucDuLieu;

    QTabWidget* tabWidget;

    // --- Tab Dashboard ---
    QLabel* lblTongNV;
    QLabel* lblDangLam;
    QLabel* lblTamNghi;
    QLabel* lblDaNghi;
    QLabel* lblCaHomNay;
    QTableWidget* tableLichHomNay;
    QTableWidget* tableNhatKy;

    // --- Tab Nhan Vien (Admin) ---
    QTableWidget* tableNhanVien;
    QLineEdit* editTimKiemNV;
    QComboBox* comboLocLoaiNV;
    QComboBox* comboLocTrangThai;
    QComboBox* comboSapXepNV;

    // --- Tab Lich Lam Viec (Admin) ---
    QDateEdit* dateLich;
    QTableWidget* tableLichNgay;
    QLineEdit* editMaCa;
    QLineEdit* editTenCa;
    QLineEdit* editGioBatDau;
    QLineEdit* editGioKetThuc;
    QComboBox* comboNVXepCa;

    // --- Tab Cham Cong (Admin) ---
    QTableWidget* tableChamCong;
    QLineEdit* editMaNVChamCong;

    // --- Tab Tinh Luong (Admin) ---
    QDateEdit* dateThangLuong;
    QTableWidget* tableLuong;
    QLabel* lblTongQuyLuong;

    // --- Tab Tai Khoan (Admin) ---
    QTableWidget* tableTaiKhoan;

    // --- Các Tab Employee ---
    QLabel* lblEmpAvatar;
    QLabel* lblEmpMaNV;
    QLabel* lblEmpHoTen;
    QLineEdit* editEmpSDT;
    QLineEdit* editEmpEmail;
    QLabel* lblEmpLoaiNV;
    QLabel* lblEmpNgayVao;
    QLineEdit* editEmpPassCu;
    QLineEdit* editEmpPassMoi;
    QLineEdit* editEmpPassXacNhan;

    QTableWidget* tableEmpLich;
    QTableWidget* tableEmpChamCong;
    QLabel* lblEmpTrangThaiChamCong;
    QLabel* lblEmpTongGio;
    QLabel* lblEmpThucLinh;
    QDateEdit* dateEmpThangLuong;

    // --- Tab Order & Ban (iPOS) ---
    QPushButton* btnDanhSachBan[30];
    int banDangChon;
    QLabel* lblThongTinBan;
    QTableWidget* tableChiTietOrder;
    QLabel* lblTongTienOrder;
    QLabel* lblTongSoMonOrder;

    // --- Tab Pha Che (KDS) ---
    QTableWidget* tableKDS;
    QLabel* lblKDSStats;

    // --- Tab Phuc Vu (Can Bung) ---
    QTableWidget* tableCanBung;
    QLabel* lblCanBungStats;

    // --- Timer dong bo Order ---
    QTimer* timerSyncOrder;

    // --- Tab Ket Ca & Doanh Thu (Admin & Thu Ngan) ---
    QDateEdit* dateKetCa;
    QTableWidget* tableKetCaNgay;
    QLabel* lblTongDauCaNgay;
    QLabel* lblTongThuNgay;
    QLabel* lblTongCuoiNgay;
    QDateEdit* dateDoanhThuThang;
    QTableWidget* tableDoanhThuThang;
    QLabel* lblTongThuThang;
    QLabel* lblTongHDThang;



public:
    MainWindow(QuanLyQuanCafe& cafeApp, QuanLyTaiKhoan& tkManager,
               const std::string& dataDir, QWidget* parent = nullptr);

private:
    void initUI();
    void buildAdminTabs();
    void buildEmployeeTabs();

    // Cập nhật dữ liệu hiển thị
    void refreshDashboard();
    void refreshTableNhanVien();
    void refreshTableLichNgay();
    void refreshTableChamCong();
    void refreshTableLuong();
    void refreshTableTaiKhoan();

    void refreshEmployeeInfo();
    void refreshEmployeeLich();
    void refreshEmployeeChamCong();
    void refreshEmployeeLuong();

    QWidget* createOrderWidget();
    QWidget* createKDSWidget();
    QWidget* createCanBungWidget();

    void refreshSoDoBan();
    void refreshChiTietBan(int soBan);
    void refreshKDS();
    void refreshCanBung();

    QWidget* createDoanhThuWidget();
    void refreshTabDoanhThu();
    void refreshBangKetCaNgay();
    void refreshBangDoanhThuThang();



private slots:
    // Admin slots
    void onThemNhanVien();
    void onSuaNhanVien();
    void onChoNghiViec();
    void onDoiTrangThai();
    void onLocVaTimKiemNV();
    void onSapXepNV();

    void onTaoCa();
    void onXepCa();
    void onXoaNhanVienKhoiCa();
    void onXoaCaBoSung();
    void onNgayLichChanged();
    void onLuuDuLieuLich();

    void onAdminChamCongVao();
    void onAdminChamCongRa();

    void onTinhLuongThang();

    void onThemTaiKhoan();
    void onResetMatKhau();
    void onXoaTaiKhoan();

    // Employee slots
    void onEmpDoiAvatar();
    void onEmpCapNhatLienHe();
    void onEmpDoiMatKhau();
    void onEmpChamCongVao();
    void onEmpChamCongRa();
    void onEmpThangLuongChanged();

    // Order & KDS slots
    void onChonBan(int soBan);
    void onThemMonVaoBan(const QString& maMon);
    void onSuaSoLuongMon(int delta);
    void onXoaMonKhoiBan();
    void onChotOrder();
    void onThanhToanBan();
    void onHuyBan();

    void onKDSXacNhan(int soBan, const QString& maMon);
    void onKDSHoanThanh(int soBan, const QString& maMon);

    void onPhuVuDaBung(int soBan, const QString& maMon);
    void onTimerSyncOrder();

    // Doanh thu & Ket ca slots
    void onNgayKetCaChanged();
    void onThangDoanhThuChanged();
    void onNhapTienDauCa();
    void onChotKetCa();


    // Hệ thống
    void onLuuDuLieu();
    void onDangXuat();
};
