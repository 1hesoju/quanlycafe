#include "MainWindow.h"
#include "AddEmployeeDialog.h"
#include "EditEmployeeDialog.h"
#include "AvatarHelper.h"
#include "LoginDialog.h"
#include "Utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <QInputDialog>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolBar>
#include <QFormLayout>
#include <QEvent>

namespace {
class WheelBlocker : public QObject {
public:
    explicit WheelBlocker(QObject* parent = nullptr) : QObject(parent) {}
protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Wheel) {
            return true; // Chặn cuộn chuột vô tình làm đổi ngày/tháng/năm
        }
        return QObject::eventFilter(obj, event);
    }
};
}

MainWindow::MainWindow(QuanLyQuanCafe& cafeApp, QuanLyTaiKhoan& tkManager,
                       const std::string& dataDir, QWidget* parent)
    : QMainWindow(parent), app(cafeApp), qlTK(tkManager), thuMucDuLieu(dataDir) {
    setWindowTitle("Phan Mem Quan Ly Nhan Vien Quan Cafe (Qt6)");
    resize(1150, 720);

    banDangChon = 1;
    lblThongTinBan = nullptr;
    tableChiTietOrder = nullptr;
    lblTongTienOrder = nullptr;
    lblTongSoMonOrder = nullptr;
    tableKDS = nullptr;
    lblKDSStats = nullptr;
    tableCanBung = nullptr;
    lblCanBungStats = nullptr;
    for (int i = 0; i < 30; ++i) btnDanhSachBan[i] = nullptr;

    initUI();

    timerSyncOrder = new QTimer(this);
    connect(timerSyncOrder, &QTimer::timeout, this, &MainWindow::onTimerSyncOrder);
    timerSyncOrder->start(2000);
}

void MainWindow::initUI() {
    // Thanh ToolBar trên cùng
    QToolBar* toolbar = addToolBar("MainToolbar");
    toolbar->setMovable(false);

    QLabel* lblUser = new QLabel(QString(" Dang dang nhap: <b>%1</b> (%2)   ")
                                 .arg(QString::fromStdString(qlTK.getTenDangNhapHienTai()))
                                 .arg(QString::fromStdString(vaiTroToChuoi(qlTK.getVaiTroHienTai()))), this);
    toolbar->addWidget(lblUser);

    QPushButton* btnSave = new QPushButton("Luu Du Lieu", this);
    btnSave->setStyleSheet("background-color: #2E7D32; color: white; padding: 5px 12px; font-weight: bold; border-radius: 3px;");
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onLuuDuLieu);
    toolbar->addWidget(btnSave);

    toolbar->addSeparator();

    QPushButton* btnLogout = new QPushButton("Dang Xuat", this);
    btnLogout->setStyleSheet("background-color: #C62828; color: white; padding: 5px 12px; font-weight: bold; border-radius: 3px;");
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onDangXuat);
    toolbar->addWidget(btnLogout);

    // Tab Widget chính
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("QTabBar::tab { height: 35px; min-width: 120px; font-weight: bold; font-size: 13px; }");
    setCentralWidget(tabWidget);

    if (qlTK.laAdmin()) {
        buildAdminTabs();
    } else {
        buildEmployeeTabs();
    }

    statusBar()->showMessage("San sang phuc vu.", 3000);
}

// =========================================================================
// GIAO DIỆN ADMIN
// =========================================================================
void MainWindow::buildAdminTabs() {
    // ----------------- TAB 1: DASHBOARD -----------------
    QWidget* tabDash = new QWidget();
    QVBoxLayout* dashLayout = new QVBoxLayout(tabDash);

    QGridLayout* cardGrid = new QGridLayout();
    auto createCard = [](const QString& title, QLabel*& valLabel, const QString& color) {
        QGroupBox* box = new QGroupBox(title);
        box->setStyleSheet(QString("QGroupBox { font-weight: bold; color: %1; border: 1px solid %1; border-radius: 6px; margin-top: 6px; } "
                                   "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }").arg(color));
        QVBoxLayout* l = new QVBoxLayout(box);
        valLabel = new QLabel("0");
        valLabel->setAlignment(Qt::AlignCenter);
        valLabel->setStyleSheet(QString("font-size: 26px; font-weight: bold; color: %1;").arg(color));
        l->addWidget(valLabel);
        return box;
    };

    cardGrid->addWidget(createCard("Tong NV Hoat Dong", lblTongNV, "#1976D2"), 0, 0);
    cardGrid->addWidget(createCard("Dang Lam Viec (Ca)", lblDangLam, "#388E3C"), 0, 1);
    cardGrid->addWidget(createCard("Tam Nghi", lblTamNghi, "#F57C00"), 0, 2);
    cardGrid->addWidget(createCard("Da Nghi Viec", lblDaNghi, "#D32F2F"), 0, 3);
    cardGrid->addWidget(createCard("So Ca Hom Nay", lblCaHomNay, "#7B1FA2"), 0, 4);
    dashLayout->addLayout(cardGrid);

    QHBoxLayout* lowerLayout = new QHBoxLayout();
    
    QGroupBox* boxLichHN = new QGroupBox("Lich Lam Viec Hom Nay");
    QVBoxLayout* vlHN = new QVBoxLayout(boxLichHN);
    tableLichHomNay = new QTableWidget();
    tableLichHomNay->setColumnCount(4);
    tableLichHomNay->setHorizontalHeaderLabels({"Ma Ca", "Ten Ca", "Khung Gio", "Danh Sach NV"});
    tableLichHomNay->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableLichHomNay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlHN->addWidget(tableLichHomNay);
    lowerLayout->addWidget(boxLichHN, 3);

    QGroupBox* boxNK = new QGroupBox("Nhat Ky Hoat Dong Gan Day");
    QVBoxLayout* vlNK = new QVBoxLayout(boxNK);
    tableNhatKy = new QTableWidget();
    tableNhatKy->setColumnCount(1);
    tableNhatKy->horizontalHeader()->setVisible(false);
    tableNhatKy->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableNhatKy->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlNK->addWidget(tableNhatKy);
    lowerLayout->addWidget(boxNK, 2);

    dashLayout->addLayout(lowerLayout);
    tabWidget->addTab(tabDash, "Dashboard");

    // ----------------- TAB 2: QUAN LY NHAN VIEN -----------------
    QWidget* tabNV = new QWidget();
    QVBoxLayout* nvLayout = new QVBoxLayout(tabNV);

    // Thanh công cụ tìm kiếm và lọc
    QHBoxLayout* filterLayout = new QHBoxLayout();
    editTimKiemNV = new QLineEdit();
    editTimKiemNV->setPlaceholderText("Tim kiem nhanh theo Ten, Ma NV, SDT, Email...");
    filterLayout->addWidget(editTimKiemNV, 3);

    comboLocLoaiNV = new QComboBox();
    comboLocLoaiNV->addItem("Tat ca vi tri", "");
    comboLocLoaiNV->addItem("Pha Che", "PHA_CHE");
    comboLocLoaiNV->addItem("Thu Ngan", "THU_NGAN");
    comboLocLoaiNV->addItem("Quan Ly", "QUAN_LY");
    comboLocLoaiNV->addItem("Phu Vu", "PHU_VU");
    comboLocLoaiNV->addItem("Bao Ve", "BAO_VE");
    filterLayout->addWidget(comboLocLoaiNV, 2);

    comboLocTrangThai = new QComboBox();
    comboLocTrangThai->addItem("Tat ca trang thai", -1);
    comboLocTrangThai->addItem("Dang lam", 0);
    comboLocTrangThai->addItem("Tam nghi", 1);
    comboLocTrangThai->addItem("Da nghi viec", 2);
    filterLayout->addWidget(comboLocTrangThai, 2);

    comboSapXepNV = new QComboBox();
    comboSapXepNV->addItem("Sap xep: Ho ten (A-Z)", 0);
    comboSapXepNV->addItem("Sap xep: Ho ten (Z-A)", 1);
    comboSapXepNV->addItem("Sap xep: Luong tang dan", 2);
    comboSapXepNV->addItem("Sap xep: Luong giam dan", 3);
    comboSapXepNV->addItem("Sap xep: Ngay vao (cu nhat)", 4);
    comboSapXepNV->addItem("Sap xep: Ngay vao (moi nhat)", 5);
    comboSapXepNV->addItem("Sap xep: Ma NV (tang dan)", 6);
    filterLayout->addWidget(comboSapXepNV, 2);

    nvLayout->addLayout(filterLayout);

    tableNhanVien = new QTableWidget();
    tableNhanVien->setColumnCount(8);
    tableNhanVien->setHorizontalHeaderLabels({"Anh", "Ma NV", "Ho va Ten", "Vi Tri", "So Dien Thoai", "Email", "Trang Thai", "Luong Co Ban"});
    tableNhanVien->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableNhanVien->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    tableNhanVien->setColumnWidth(0, 56);
    tableNhanVien->verticalHeader()->setDefaultSectionSize(46);
    tableNhanVien->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableNhanVien->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(tableNhanVien, &QTableWidget::cellDoubleClicked, this, &MainWindow::onSuaNhanVien);
    nvLayout->addWidget(tableNhanVien);

    // Hàng nút chức năng nhân viên
    QHBoxLayout* nvBtnLayout = new QHBoxLayout();
    QPushButton* btnAddNV = new QPushButton("+ Them Nhan Vien");
    btnAddNV->setStyleSheet("background-color: #6F4E37; color: white; font-weight: bold; padding: 6px 14px; border-radius: 4px;");
    QPushButton* btnEditNV = new QPushButton("Sua Thong Tin");
    QPushButton* btnStatusNV = new QPushButton("Doi Trang Thai");
    QPushButton* btnDeleteNV = new QPushButton("Cho Nghi Viec (Xoa mem)");
    btnDeleteNV->setStyleSheet("color: #D32F2F;");

    nvBtnLayout->addWidget(btnAddNV);
    nvBtnLayout->addWidget(btnEditNV);
    nvBtnLayout->addWidget(btnStatusNV);
    nvBtnLayout->addWidget(btnDeleteNV);
    nvBtnLayout->addStretch();
    nvLayout->addLayout(nvBtnLayout);

    tabWidget->addTab(tabNV, "Nhan Vien");

    // ----------------- TAB 3: LICH LAM VIEC -----------------
    QWidget* tabLich = new QWidget();
    QVBoxLayout* lichLayout = new QVBoxLayout(tabLich);

    QHBoxLayout* datePickLayout = new QHBoxLayout();
    datePickLayout->addWidget(new QLabel("Chon ngay xem & sap xep lich:"));
    dateLich = new QDateEdit(QDate::currentDate());
    dateLich->setCalendarPopup(true);
    dateLich->setDisplayFormat("dd/MM/yyyy");
    dateLich->installEventFilter(new WheelBlocker(dateLich));
    datePickLayout->addWidget(dateLich);

    QPushButton* btnLuuLich = new QPushButton("Luu Du Lieu Lich Lam", this);
    btnLuuLich->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; padding: 5px 14px; border-radius: 3px;");
    datePickLayout->addWidget(btnLuuLich);

    QLabel* lblGhiChu = new QLabel(" (1 NV co the lam nhieu ca khong trung gio trong ngay)");
    lblGhiChu->setStyleSheet("color: #2E7D32; font-style: italic;");
    datePickLayout->addWidget(lblGhiChu);
    datePickLayout->addStretch();
    lichLayout->addLayout(datePickLayout);

    tableLichNgay = new QTableWidget();
    tableLichNgay->setColumnCount(5);
    tableLichNgay->setHorizontalHeaderLabels({"Ma Ca", "Ten Ca", "Gio Bat Dau", "Gio Ket Thuc", "Danh Sach Nhan Vien Lam Ca"});
    tableLichNgay->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableLichNgay->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableLichNgay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lichLayout->addWidget(tableLichNgay);

    // Khu vực Xếp nhân viên vào ca & Tạo ca bổ sung
    QHBoxLayout* caActionLayout = new QHBoxLayout();

    // 1. Phân công nhân viên vào ca
    QGroupBox* boxXepCa = new QGroupBox("Phan Cong Nhan Vien Vao Ca");
    QVBoxLayout* vXepCa = new QVBoxLayout(boxXepCa);
    
    QHBoxLayout* hChonNV = new QHBoxLayout();
    hChonNV->addWidget(new QLabel("Chon NV:"));
    comboNVXepCa = new QComboBox();
    comboNVXepCa->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hChonNV->addWidget(comboNVXepCa);
    vXepCa->addLayout(hChonNV);

    QHBoxLayout* hBtnXep = new QHBoxLayout();
    QPushButton* btnXepCa = new QPushButton("+ Chi Dinh Vao Ca Dang Chon");
    btnXepCa->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; padding: 6px; border-radius: 3px;");
    QPushButton* btnXoaNV = new QPushButton("- Xoa NV Khoi Ca");
    btnXoaNV->setStyleSheet("background-color: #D32F2F; color: white; padding: 6px; border-radius: 3px;");
    hBtnXep->addWidget(btnXepCa);
    hBtnXep->addWidget(btnXoaNV);
    vXepCa->addLayout(hBtnXep);

    caActionLayout->addWidget(boxXepCa, 3);

    // 2. Tạo ca bổ sung riêng biệt
    QGroupBox* boxTaoCa = new QGroupBox("Tao Ca Bo Sung Rieng (Dac biet)");
    QGridLayout* gTaoCa = new QGridLayout(boxTaoCa);
    
    editMaCa = new QLineEdit(); editMaCa->setPlaceholderText("Ma Ca (VD: CA_BS01)");
    editTenCa = new QLineEdit(); editTenCa->setPlaceholderText("Ten Ca (VD: Ca Bo Sung Cuoi Tuan)");
    editGioBatDau = new QLineEdit(); editGioBatDau->setPlaceholderText("Gio bat dau (06:00)");
    editGioKetThuc = new QLineEdit(); editGioKetThuc->setPlaceholderText("Gio ket thuc (09:00)");
    QPushButton* btnTaoCa = new QPushButton("+ Tao Ca Bo Sung");
    btnTaoCa->setStyleSheet("background-color: #6F4E37; color: white; font-weight: bold; padding: 6px; border-radius: 3px;");
    QPushButton* btnXoaCa = new QPushButton("Xoa Ca Bo Sung");
    btnXoaCa->setStyleSheet("color: #C62828; padding: 6px;");

    gTaoCa->addWidget(new QLabel("Ma ca:"), 0, 0);
    gTaoCa->addWidget(editMaCa, 0, 1);
    gTaoCa->addWidget(new QLabel("Ten ca:"), 0, 2);
    gTaoCa->addWidget(editTenCa, 0, 3);
    gTaoCa->addWidget(new QLabel("Tu gio:"), 1, 0);
    gTaoCa->addWidget(editGioBatDau, 1, 1);
    gTaoCa->addWidget(new QLabel("Den gio:"), 1, 2);
    gTaoCa->addWidget(editGioKetThuc, 1, 3);
    gTaoCa->addWidget(btnTaoCa, 2, 0, 1, 2);
    gTaoCa->addWidget(btnXoaCa, 2, 2, 1, 2);

    caActionLayout->addWidget(boxTaoCa, 4);

    lichLayout->addLayout(caActionLayout);
    tabWidget->addTab(tabLich, "Lich Lam Viec");

    // ----------------- TAB 4: CHAM CONG -----------------
    QWidget* tabCC = new QWidget();
    QVBoxLayout* ccLayout = new QVBoxLayout(tabCC);

    QHBoxLayout* ccTop = new QHBoxLayout();
    editMaNVChamCong = new QLineEdit();
    editMaNVChamCong->setPlaceholderText("Nhap Ma NV de cham cong hoac loc lich su (VD: NV1)...");
    QPushButton* btnVao = new QPushButton("Cham Cong VAO");
    btnVao->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold;");
    QPushButton* btnRa = new QPushButton("Cham Cong RA");
    btnRa->setStyleSheet("background-color: #C62828; color: white; font-weight: bold;");
    ccTop->addWidget(editMaNVChamCong, 2);
    ccTop->addWidget(btnVao, 1);
    ccTop->addWidget(btnRa, 1);
    ccLayout->addLayout(ccTop);

    tableChamCong = new QTableWidget();
    tableChamCong->setColumnCount(6);
    tableChamCong->setHorizontalHeaderLabels({"Ma Nhan Vien", "Ngay", "Ma Ca", "Gio Vao", "Gio Ra", "Phan Loai"});
    tableChamCong->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableChamCong->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ccLayout->addWidget(tableChamCong);

    tabWidget->addTab(tabCC, "Cham Cong");

    // ----------------- TAB 5: TINH LUONG -----------------
    QWidget* tabLuong = new QWidget();
    QVBoxLayout* luongLayout = new QVBoxLayout(tabLuong);

    QHBoxLayout* luongTop = new QHBoxLayout();
    luongTop->addWidget(new QLabel("Chon Thang/Nam:"));
    dateThangLuong = new QDateEdit(QDate::currentDate());
    dateThangLuong->setDisplayFormat("MM/yyyy");
    dateThangLuong->installEventFilter(new WheelBlocker(dateThangLuong));
    luongTop->addWidget(dateThangLuong);

    QPushButton* btnCalc = new QPushButton("Tinh Bang Luong Thang");
    btnCalc->setStyleSheet("background-color: #6F4E37; color: white; font-weight: bold; padding: 5px 12px;");
    luongTop->addWidget(btnCalc);

    lblTongQuyLuong = new QLabel("Tong Quy Luong: <b>0 VND</b>");
    lblTongQuyLuong->setStyleSheet("font-size: 15px; color: #2E7D32;");
    luongTop->addStretch();
    luongTop->addWidget(lblTongQuyLuong);
    luongLayout->addLayout(luongTop);

    tableLuong = new QTableWidget();
    tableLuong->setColumnCount(4);
    tableLuong->setHorizontalHeaderLabels({"Ma Nhan Vien", "Thang / Nam", "Tong So Gio Lam", "Thuc Linh (VND)"});
    tableLuong->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableLuong->setEditTriggers(QAbstractItemView::NoEditTriggers);
    luongLayout->addWidget(tableLuong);

    tabWidget->addTab(tabLuong, "Tinh Luong");

    // ----------------- TAB 6: TAI KHOAN -----------------
    QWidget* tabTK = new QWidget();
    QVBoxLayout* tkLayout = new QVBoxLayout(tabTK);

    tableTaiKhoan = new QTableWidget();
    tableTaiKhoan->setColumnCount(3);
    tableTaiKhoan->setHorizontalHeaderLabels({"Ten Dang Nhap", "Vai Tro", "Ma Nhan Vien Lien Ket"});
    tableTaiKhoan->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableTaiKhoan->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTaiKhoan->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tkLayout->addWidget(tableTaiKhoan);

    QHBoxLayout* tkBtnLayout = new QHBoxLayout();
    QPushButton* btnThemTK = new QPushButton("+ Them Tai Khoan");
    QPushButton* btnResetPW = new QPushButton("Reset Mat Khau Ve 123456");
    QPushButton* btnXoaTK = new QPushButton("Xoa Tai Khoan");
    btnXoaTK->setStyleSheet("color: #C62828;");

    tkBtnLayout->addWidget(btnThemTK);
    tkBtnLayout->addWidget(btnResetPW);
    tkBtnLayout->addWidget(btnXoaTK);
    tkBtnLayout->addStretch();
    tkLayout->addLayout(tkBtnLayout);

    tabWidget->addTab(tabTK, "Tai Khoan");

    // Kết nối Signal - Slots cho Admin
    connect(editTimKiemNV, &QLineEdit::textChanged, this, &MainWindow::onLocVaTimKiemNV);
    connect(comboLocLoaiNV, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onLocVaTimKiemNV);
    connect(comboLocTrangThai, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onLocVaTimKiemNV);
    connect(comboSapXepNV, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSapXepNV);

    connect(btnAddNV, &QPushButton::clicked, this, &MainWindow::onThemNhanVien);
    connect(btnEditNV, &QPushButton::clicked, this, &MainWindow::onSuaNhanVien);
    connect(btnStatusNV, &QPushButton::clicked, this, &MainWindow::onDoiTrangThai);
    connect(btnDeleteNV, &QPushButton::clicked, this, &MainWindow::onChoNghiViec);

    connect(dateLich, &QDateEdit::dateChanged, this, &MainWindow::onNgayLichChanged);
    connect(btnTaoCa, &QPushButton::clicked, this, &MainWindow::onTaoCa);
    connect(btnXoaCa, &QPushButton::clicked, this, &MainWindow::onXoaCaBoSung);
    connect(btnXepCa, &QPushButton::clicked, this, &MainWindow::onXepCa);
    connect(btnXoaNV, &QPushButton::clicked, this, &MainWindow::onXoaNhanVienKhoiCa);
    connect(btnLuuLich, &QPushButton::clicked, this, &MainWindow::onLuuDuLieuLich);

    connect(btnVao, &QPushButton::clicked, this, &MainWindow::onAdminChamCongVao);
    connect(btnRa, &QPushButton::clicked, this, &MainWindow::onAdminChamCongRa);

    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onTinhLuongThang);

    connect(btnThemTK, &QPushButton::clicked, this, &MainWindow::onThemTaiKhoan);
    connect(btnResetPW, &QPushButton::clicked, this, &MainWindow::onResetMatKhau);
    connect(btnXoaTK, &QPushButton::clicked, this, &MainWindow::onXoaTaiKhoan);

    // Tab Order Mon va Quay Pha Che cho Admin
    tabWidget->addTab(createOrderWidget(), "Order Mon (iPOS)");
    tabWidget->addTab(createKDSWidget(), "Quay Pha Che (KDS)");

    // Nạp dữ liệu ban đầu
    refreshDashboard();
    refreshTableNhanVien();
    refreshTableLichNgay();
    refreshTableChamCong();
    refreshTableLuong();
    refreshTableTaiKhoan();
}

// =========================================================================
// GIAO DIỆN EMPLOYEE
// =========================================================================
void MainWindow::buildEmployeeTabs() {
    std::string maNV = qlTK.getMaNVHienTai();
    NhanVien* nv = app.timNhanVien(maNV);

    // ----------------- TAB 1: THONG TIN CA NHAN -----------------
    QWidget* tabProfile = new QWidget();
    QHBoxLayout* hProfile = new QHBoxLayout(tabProfile);

    QGroupBox* boxInfo = new QGroupBox("Ho So Nhan Vien");
    QVBoxLayout* vbInfo = new QVBoxLayout(boxInfo);
    vbInfo->setSpacing(12);

    QHBoxLayout* empAvatarLayout = new QHBoxLayout();
    empAvatarLayout->setSpacing(14);
    lblEmpAvatar = new QLabel(this);
    lblEmpAvatar->setFixedSize(88, 88);
    lblEmpAvatar->setAlignment(Qt::AlignCenter);
    empAvatarLayout->addWidget(lblEmpAvatar);

    QVBoxLayout* empAvatarBtnLayout = new QVBoxLayout();
    empAvatarBtnLayout->setAlignment(Qt::AlignVCenter);
    QPushButton* btnEmpDoiAvatar = new QPushButton("Doi Anh Dai Dien", this);
    btnEmpDoiAvatar->setFixedHeight(32);
    btnEmpDoiAvatar->setStyleSheet("background-color: #8D6E63; color: white; border-radius: 4px; padding: 0 12px; font-weight: 500;");
    connect(btnEmpDoiAvatar, &QPushButton::clicked, this, &MainWindow::onEmpDoiAvatar);
    empAvatarBtnLayout->addWidget(btnEmpDoiAvatar);
    empAvatarLayout->addLayout(empAvatarBtnLayout);
    empAvatarLayout->addStretch();
    vbInfo->addLayout(empAvatarLayout);

    QFormLayout* fInfo = new QFormLayout();
    fInfo->setSpacing(10);

    lblEmpMaNV = new QLabel(QString::fromStdString(maNV));
    lblEmpHoTen = new QLabel(nv ? QString::fromStdString(nv->getHoTen()) : "");
    lblEmpLoaiNV = new QLabel(nv ? QString::fromStdString(nv->loaiNhanVien()) : "");
    lblEmpNgayVao = new QLabel(nv ? QString::fromStdString(nv->getNgayVaoLam()) : "");

    editEmpSDT = new QLineEdit(nv ? QString::fromStdString(nv->getSdt()) : "");
    editEmpEmail = new QLineEdit(nv ? QString::fromStdString(nv->getEmail()) : "");
    QPushButton* btnUpdateContact = new QPushButton("Luu So Dien Thoai & Email");
    btnUpdateContact->setStyleSheet("background-color: #6F4E37; color: white;");

    fInfo->addRow("Ma nhan vien:", lblEmpMaNV);
    fInfo->addRow("Ho va ten:", lblEmpHoTen);
    fInfo->addRow("Vi tri:", lblEmpLoaiNV);
    fInfo->addRow("Ngay vao lam:", lblEmpNgayVao);
    fInfo->addRow("So dien thoai:", editEmpSDT);
    fInfo->addRow("Email:", editEmpEmail);
    fInfo->addRow("", btnUpdateContact);
    vbInfo->addLayout(fInfo);
    hProfile->addWidget(boxInfo);

    QGroupBox* boxPass = new QGroupBox("Doi Mat Khau Ca Nhan");
    QFormLayout* fPass = new QFormLayout(boxPass);
    fPass->setSpacing(12);

    editEmpPassCu = new QLineEdit(); editEmpPassCu->setEchoMode(QLineEdit::Password);
    editEmpPassMoi = new QLineEdit(); editEmpPassMoi->setEchoMode(QLineEdit::Password);
    editEmpPassXacNhan = new QLineEdit(); editEmpPassXacNhan->setEchoMode(QLineEdit::Password);
    QPushButton* btnChangePass = new QPushButton("Doi Mat Khau");
    btnChangePass->setStyleSheet("background-color: #2E7D32; color: white;");

    fPass->addRow("Mat khau cu:", editEmpPassCu);
    fPass->addRow("Mat khau moi:", editEmpPassMoi);
    fPass->addRow("Xac nhan mat khau:", editEmpPassXacNhan);
    fPass->addRow("", btnChangePass);
    hProfile->addWidget(boxPass);

    tabWidget->addTab(tabProfile, "Thong Tin Ca Nhan");

    // ----------------- TAB 2: LICH CUA TOI -----------------
    QWidget* tabLich = new QWidget();
    QVBoxLayout* vlLich = new QVBoxLayout(tabLich);
    tableEmpLich = new QTableWidget();
    tableEmpLich->setColumnCount(4);
    tableEmpLich->setHorizontalHeaderLabels({"Ngay", "Ma Ca", "Ten Ca", "Khung Gio"});
    tableEmpLich->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableEmpLich->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlLich->addWidget(tableEmpLich);
    tabWidget->addTab(tabLich, "Lich Lam Cua Toi");

    // ----------------- TAB 3: CHAM CONG CUA TOI -----------------
    QWidget* tabCC = new QWidget();
    QVBoxLayout* vlCC = new QVBoxLayout(tabCC);

    QHBoxLayout* ccBtnLayout = new QHBoxLayout();
    lblEmpTrangThaiChamCong = new QLabel("Trang thai: Chua check-in");
    lblEmpTrangThaiChamCong->setStyleSheet("font-size: 14px; font-weight: bold;");
    QPushButton* btnCheckIn = new QPushButton("Cham Cong VAO");
    btnCheckIn->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; font-size: 14px; height: 40px;");
    QPushButton* btnCheckOut = new QPushButton("Cham Cong RA");
    btnCheckOut->setStyleSheet("background-color: #C62828; color: white; font-weight: bold; font-size: 14px; height: 40px;");

    ccBtnLayout->addWidget(lblEmpTrangThaiChamCong, 2);
    ccBtnLayout->addWidget(btnCheckIn, 1);
    ccBtnLayout->addWidget(btnCheckOut, 1);
    vlCC->addLayout(ccBtnLayout);

    tableEmpChamCong = new QTableWidget();
    tableEmpChamCong->setColumnCount(5);
    tableEmpChamCong->setHorizontalHeaderLabels({"Ngay", "Ma Ca", "Gio Vao", "Gio Ra", "Phan Loai"});
    tableEmpChamCong->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableEmpChamCong->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlCC->addWidget(tableEmpChamCong);
    tabWidget->addTab(tabCC, "Cham Cong Cua Toi");

    // ----------------- TAB 4: LUONG CUA TOI -----------------
    QWidget* tabLuong = new QWidget();
    QVBoxLayout* vlLuong = new QVBoxLayout(tabLuong);

    QHBoxLayout* luongTop = new QHBoxLayout();
    luongTop->addWidget(new QLabel("Chon Thang/Nam:"));
    dateEmpThangLuong = new QDateEdit(QDate::currentDate());
    dateEmpThangLuong->setDisplayFormat("MM/yyyy");
    dateEmpThangLuong->installEventFilter(new WheelBlocker(dateEmpThangLuong));
    luongTop->addWidget(dateEmpThangLuong);
    luongTop->addStretch();
    vlLuong->addLayout(luongTop);

    QGroupBox* boxSummary = new QGroupBox("Bang Luong Thang");
    QVBoxLayout* vSummary = new QVBoxLayout(boxSummary);
    lblEmpTongGio = new QLabel("Tong so gio lam: 0 gio");
    lblEmpTongGio->setStyleSheet("font-size: 16px; margin: 10px;");
    lblEmpThucLinh = new QLabel("Thuc linh: 0 VND");
    lblEmpThucLinh->setStyleSheet("font-size: 20px; font-weight: bold; color: #2E7D32; margin: 10px;");
    vSummary->addWidget(lblEmpTongGio);
    vSummary->addWidget(lblEmpThucLinh);
    vlLuong->addWidget(boxSummary);
    vlLuong->addStretch();

    tabWidget->addTab(tabLuong, "Bang Luong Cua Toi");

    // Kết nối Signal - Slots cho Employee
    connect(btnUpdateContact, &QPushButton::clicked, this, &MainWindow::onEmpCapNhatLienHe);
    connect(btnChangePass, &QPushButton::clicked, this, &MainWindow::onEmpDoiMatKhau);
    connect(btnCheckIn, &QPushButton::clicked, this, &MainWindow::onEmpChamCongVao);
    connect(btnCheckOut, &QPushButton::clicked, this, &MainWindow::onEmpChamCongRa);
    connect(dateEmpThangLuong, &QDateEdit::dateChanged, this, &MainWindow::onEmpThangLuongChanged);

    // Them cac tab nghiep vu phu hop voi tung vi tri
    std::string role = nv ? nv->loaiNhanVien() : "";
    if (role == "PHU_VU") {
        tabWidget->addTab(createOrderWidget(), "Ban & Order (iPOS)");
        tabWidget->addTab(createCanBungWidget(), "Mon Can Bung (Phuc Vu)");
    } else if (role == "THU_NGAN") {
        tabWidget->addTab(createOrderWidget(), "Ban & Order (iPOS)");
    } else if (role == "PHA_CHE") {
        tabWidget->addTab(createKDSWidget(), "Quay Pha Che (KDS)");
    } else if (role == "QUAN_LY") {
        tabWidget->addTab(createOrderWidget(), "Ban & Order (iPOS)");
        tabWidget->addTab(createKDSWidget(), "Quay Pha Che (KDS)");
    }

    refreshEmployeeInfo();
    refreshEmployeeLich();
    refreshEmployeeChamCong();
    refreshEmployeeLuong();
}

// =========================================================================
// REFRESH DATA FUNCTIONS
// =========================================================================
void MainWindow::refreshDashboard() {
    auto dt = app.layDashboard();
    lblTongNV->setText(QString::number(dt.tongNhanVien));
    lblDangLam->setText(QString::number(dt.dangLamViec));
    lblTamNghi->setText(QString::number(dt.tamNghi));
    lblDaNghi->setText(QString::number(dt.daNghiViec));
    lblCaHomNay->setText(QString::number(dt.soCaHomNay));

    // Lịch hôm nay
    auto dsCa = app.layLich().xemLichNgay(Utils::ngayHomNay());
    tableLichHomNay->setRowCount(dsCa.size());
    for (size_t i = 0; i < dsCa.size(); ++i) {
        tableLichHomNay->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(dsCa[i].getMaCa())));
        tableLichHomNay->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(dsCa[i].getTenCa())));
        tableLichHomNay->setItem(i, 2, new QTableWidgetItem(QString("%1 - %2").arg(QString::fromStdString(dsCa[i].getGioBatDau()), QString::fromStdString(dsCa[i].getGioKetThuc()))));

        QStringList nvList;
        for (const auto& m : dsCa[i].getDsNhanVien()) nvList << QString::fromStdString(m);
        tableLichHomNay->setItem(i, 3, new QTableWidgetItem(nvList.join(", ")));
    }

    // Nhật ký
    auto nk = app.layNhatKyGanDay();
    tableNhatKy->setRowCount(nk.size());
    for (size_t i = 0; i < nk.size(); ++i) {
        tableNhatKy->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(nk[nk.size() - 1 - i]))); // đảo mới nhất lên đầu
    }
}

void MainWindow::refreshTableNhanVien() {
    onLocVaTimKiemNV();
}

void MainWindow::onLocVaTimKiemNV() {
    BoLocNhanVien bl;
    bl.tuKhoa = editTimKiemNV->text().trimmed().toStdString();
    bl.loaiNV = comboLocLoaiNV->currentData().toString().toStdString();
    bl.trangThai = comboLocTrangThai->currentData().toInt();

    auto ds = app.timKiemVaLoc(bl);

    // Sắp xếp
    int sortIdx = comboSapXepNV->currentIndex();
    TieuChiSapXep tc = static_cast<TieuChiSapXep>(sortIdx);
    app.sapXepNhanVien(ds, tc);

    tableNhanVien->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        QLabel* lblImg = new QLabel();
        lblImg->setAlignment(Qt::AlignCenter);
        lblImg->setPixmap(AvatarHelper::getAvatarPixmap(ds[i]->getAvatar(), ds[i]->getHoTen(), 36));
        tableNhanVien->setCellWidget(i, 0, lblImg);

        tableNhanVien->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i]->getMaNV())));
        tableNhanVien->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i]->getHoTen())));
        tableNhanVien->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(ds[i]->loaiNhanVien())));
        tableNhanVien->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(ds[i]->getSdt())));
        tableNhanVien->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(ds[i]->getEmail())));
        tableNhanVien->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(trangThaiToChuoi(ds[i]->getTrangThai()))));
        tableNhanVien->setItem(i, 7, new QTableWidgetItem(QString("%L1 VND").arg(ds[i]->getLuongCoBan(), 0, 'f', 0)));
    }
}

void MainWindow::onSapXepNV() {
    onLocVaTimKiemNV();
}

void MainWindow::refreshTableLichNgay() {
    std::string ngay = dateLich->date().toString("dd/MM/yyyy").toStdString();
    
    // Tự động đảm bảo ngày nào cũng có 3 ca cố định (Sáng 07-12h, Chiều 12-17h, Tối 17-22h)
    app.layLich().khoiTao3CaCoDinhNeuChuaCo(ngay);

    // Cập nhật danh sách nhân viên vào ComboBox để Admin tiện chỉ định
    if (comboNVXepCa) {
        QString currentSelected = comboNVXepCa->currentData().toString();
        comboNVXepCa->clear();
        for (const auto& nv : app.layDanhSachNV()) {
            if (nv->getTrangThai() != TrangThaiNV::DA_NGHI_VIEC) {
                QString label = QString("%1 - %2 (%3)")
                                .arg(QString::fromStdString(nv->getMaNV()))
                                .arg(QString::fromStdString(nv->getHoTen()))
                                .arg(QString::fromStdString(nv->loaiNhanVien()));
                comboNVXepCa->addItem(label, QString::fromStdString(nv->getMaNV()));
            }
        }
        int foundIdx = comboNVXepCa->findData(currentSelected);
        if (foundIdx >= 0) comboNVXepCa->setCurrentIndex(foundIdx);
    }

    auto ds = app.layLich().xemLichNgay(ngay);
    tableLichNgay->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        tableLichNgay->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].getMaCa())));
        tableLichNgay->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i].getTenCa())));
        tableLichNgay->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i].getGioBatDau())));
        tableLichNgay->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(ds[i].getGioKetThuc())));

        // Hiển thị mã NV kèm tên để dễ nhìn
        QStringList nvList;
        for (const auto& m : ds[i].getDsNhanVien()) {
            NhanVien* emp = app.timNhanVien(m);
            if (emp) {
                nvList << QString("%1 (%2)").arg(QString::fromStdString(m), QString::fromStdString(emp->getHoTen()));
            } else {
                nvList << QString::fromStdString(m);
            }
        }
        tableLichNgay->setItem(i, 4, new QTableWidgetItem(nvList.join(", ")));
    }
}

void MainWindow::onNgayLichChanged() {
    refreshTableLichNgay();
}

void MainWindow::refreshTableChamCong() {
    std::string maNV = editMaNVChamCong ? editMaNVChamCong->text().trimmed().toStdString() : "";
    std::vector<BanGhiChamCong> ds;
    if (maNV.empty()) {
        ds = app.layChamCong().lichSuNgay(Utils::ngayHomNay());
    } else {
        ds = app.layChamCong().lichSuNhanVien(maNV);
    }

    tableChamCong->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        tableChamCong->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].maNV)));
        tableChamCong->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i].ngay)));
        tableChamCong->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i].maCa.empty() ? "-" : ds[i].maCa)));
        tableChamCong->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(ds[i].gioVao)));
        tableChamCong->setItem(i, 4, new QTableWidgetItem(ds[i].gioRa.empty() ? "(Dang lam)" : QString::fromStdString(ds[i].gioRa)));

        // Cột Phân Loại trạng thái
        bool dangLam = ds[i].gioRa.empty();
        QString ttStr = dangLam ? "Dang lam..." : QString::fromStdString(ds[i].trangThai.empty() ? "Dung gio" : ds[i].trangThai);
        QTableWidgetItem* itTT = new QTableWidgetItem(ttStr);
        itTT->setTextAlignment(Qt::AlignCenter);
        QFont f = itTT->font();
        f.setBold(true);
        itTT->setFont(f);
        if (dangLam) {
            itTT->setForeground(QBrush(QColor("#1565C0")));
        } else if (ds[i].trangThai == "Dung gio" || ds[i].trangThai.empty()) {
            itTT->setForeground(QBrush(QColor("#2E7D32")));
        } else if (ds[i].trangThai == "Di muon") {
            itTT->setForeground(QBrush(QColor("#E65100")));
        } else if (ds[i].trangThai == "Ve som") {
            itTT->setForeground(QBrush(QColor("#D32F2F")));
        } else {
            itTT->setForeground(QBrush(QColor("#AD1457")));
        }
        tableChamCong->setItem(i, 5, itTT);
    }
}

void MainWindow::refreshTableLuong() {
    std::string thang = dateThangLuong->date().toString("MM/yyyy").toStdString();
    auto ds = app.bangLuongThang(thang);
    tableLuong->setRowCount(ds.size());
    double tong = 0;
    for (size_t i = 0; i < ds.size(); ++i) {
        tableLuong->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].maNV)));
        tableLuong->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i].thangNam)));
        tableLuong->setItem(i, 2, new QTableWidgetItem(QString("%1 gio").arg(ds[i].soGioLam, 0, 'f', 1)));
        tableLuong->setItem(i, 3, new QTableWidgetItem(QString("%L1 VND").arg(ds[i].thucLinh, 0, 'f', 0)));
        tong += ds[i].thucLinh;
    }
    lblTongQuyLuong->setText(QString("Tong Quy Luong: <b>%L1 VND</b>").arg(tong, 0, 'f', 0));
}

void MainWindow::refreshTableTaiKhoan() {
    const auto& ds = qlTK.layDanhSachTK();
    tableTaiKhoan->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        tableTaiKhoan->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].getTenDangNhap())));
        tableTaiKhoan->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vaiTroToChuoi(ds[i].getVaiTro()))));
        tableTaiKhoan->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i].getMaNV())));
    }
}

// =========================================================================
// EMPLOYEE REFRESH
// =========================================================================
void MainWindow::refreshEmployeeInfo() {
    std::string maNV = qlTK.getMaNVHienTai();
    NhanVien* nv = app.timNhanVien(maNV);
    if (!nv) return;
    lblEmpAvatar->setPixmap(AvatarHelper::getAvatarPixmap(nv->getAvatar(), nv->getHoTen(), 88));
    lblEmpMaNV->setText(QString::fromStdString(nv->getMaNV()));
    lblEmpHoTen->setText(QString::fromStdString(nv->getHoTen()));
    lblEmpLoaiNV->setText(QString::fromStdString(nv->loaiNhanVien()));
    lblEmpNgayVao->setText(QString::fromStdString(nv->getNgayVaoLam()));
    editEmpSDT->setText(QString::fromStdString(nv->getSdt()));
    editEmpEmail->setText(QString::fromStdString(nv->getEmail()));
}

void MainWindow::refreshEmployeeLich() {
    std::string maNV = qlTK.getMaNVHienTai();
    auto ds = app.layLich().xemLichNhanVien(maNV);
    tableEmpLich->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        tableEmpLich->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].getNgay())));
        tableEmpLich->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i].getMaCa())));
        tableEmpLich->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i].getTenCa())));
        tableEmpLich->setItem(i, 3, new QTableWidgetItem(QString("%1 - %2").arg(QString::fromStdString(ds[i].getGioBatDau()), QString::fromStdString(ds[i].getGioKetThuc()))));
    }
}

void MainWindow::refreshEmployeeChamCong() {
    std::string maNV = qlTK.getMaNVHienTai();
    bool dangLam = app.layChamCong().dangTrongCa(maNV);
    if (dangLam) {
        lblEmpTrangThaiChamCong->setText("Trang thai: <font color='#2E7D32'>Dang trong ca lam viec</font>");
    } else {
        lblEmpTrangThaiChamCong->setText("Trang thai: <font color='#757575'>Chua check-in</font>");
    }

    auto ds = app.layChamCong().lichSuNhanVien(maNV);
    tableEmpChamCong->setRowCount(ds.size());
    for (size_t i = 0; i < ds.size(); ++i) {
        tableEmpChamCong->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ds[i].ngay)));
        tableEmpChamCong->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ds[i].maCa.empty() ? "-" : ds[i].maCa)));
        tableEmpChamCong->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ds[i].gioVao)));
        tableEmpChamCong->setItem(i, 3, new QTableWidgetItem(ds[i].gioRa.empty() ? "(Dang lam)" : QString::fromStdString(ds[i].gioRa)));

        // Cột Phân Loại trạng thái
        bool dangLamRow = ds[i].gioRa.empty();
        QString ttStr = dangLamRow ? "Dang lam..." : QString::fromStdString(ds[i].trangThai.empty() ? "Dung gio" : ds[i].trangThai);
        QTableWidgetItem* itTT = new QTableWidgetItem(ttStr);
        itTT->setTextAlignment(Qt::AlignCenter);
        QFont f = itTT->font();
        f.setBold(true);
        itTT->setFont(f);
        if (dangLamRow) {
            itTT->setForeground(QBrush(QColor("#1565C0")));
        } else if (ds[i].trangThai == "Dung gio" || ds[i].trangThai.empty()) {
            itTT->setForeground(QBrush(QColor("#2E7D32")));
        } else if (ds[i].trangThai == "Di muon") {
            itTT->setForeground(QBrush(QColor("#E65100")));
        } else if (ds[i].trangThai == "Ve som") {
            itTT->setForeground(QBrush(QColor("#D32F2F")));
        } else {
            itTT->setForeground(QBrush(QColor("#AD1457")));
        }
        tableEmpChamCong->setItem(i, 4, itTT);
    }
}

void MainWindow::refreshEmployeeLuong() {
    std::string maNV = qlTK.getMaNVHienTai();
    std::string thang = dateEmpThangLuong->date().toString("MM/yyyy").toStdString();
    double luong = app.tinhLuongThang(maNV, thang);
    double gio = app.layChamCong().tongGioLamThang(maNV, thang);

    lblEmpTongGio->setText(QString("Tong so gio lam thang %1: <b>%2 gio</b>").arg(QString::fromStdString(thang)).arg(gio, 0, 'f', 1));
    lblEmpThucLinh->setText(QString("Thuc linh: <b>%L1 VND</b>").arg(luong >= 0 ? luong : 0, 0, 'f', 0));
}

// =========================================================================
// SLOTS XỬ LÝ SỰ KIỆN
// =========================================================================
void MainWindow::onThemNhanVien() {
    std::string maMoi = app.sinhMaNVMoi();
    AddEmployeeDialog dlg(maMoi, thuMucDuLieu, this);
    if (dlg.exec() == QDialog::Accepted) {
        auto nv = dlg.getNhanVienMoi();
        if (nv) {
            std::string maNV = nv->getMaNV();
            if (app.themNhanVien(std::move(nv))) {
                qlTK.themTaiKhoan(maNV, "123456", VaiTro::EMPLOYEE, maNV);
                onLuuDuLieu();
                refreshDashboard();
                refreshTableNhanVien();
                refreshTableTaiKhoan();
                QMessageBox::information(this, "Thanh cong", QString("Da them nhan vien %1 thanh cong!\nTai khoan: %1 (Mat khau: 123456)").arg(QString::fromStdString(maNV)));
            } else {
                QMessageBox::warning(this, "That bai", "Khong the them nhan vien (trung ma)!");
            }
        }
    }
}

void MainWindow::onSuaNhanVien() {
    int row = tableNhanVien->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 nhan vien trong bang de sua!");
        return;
    }
    std::string maNV = tableNhanVien->item(row, 1)->text().toStdString();
    NhanVien* nv = app.timNhanVien(maNV);
    if (!nv) return;

    EditEmployeeDialog dlg(nv, thuMucDuLieu, this);
    if (dlg.exec() == QDialog::Accepted) {
        onLuuDuLieu();
        refreshDashboard();
        refreshTableNhanVien();
        QMessageBox::information(this, "Thanh cong", "Da cap nhat thong tin nhan vien thanh cong!");
    }
}

void MainWindow::onDoiTrangThai() {
    int row = tableNhanVien->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 nhan vien de doi trang thai!");
        return;
    }
    std::string maNV = tableNhanVien->item(row, 1)->text().toStdString();
    NhanVien* nv = app.timNhanVien(maNV);
    if (!nv) return;

    QStringList items = {"Dang lam", "Tam nghi"};
    bool ok = false;
    QString item = QInputDialog::getItem(this, "Doi Trang Thai", "Chon trang thai:", items, nv->getTrangThai() == TrangThaiNV::TAM_NGHI ? 1 : 0, false, &ok);
    if (ok) {
        nv->setTrangThai(item == "Tam nghi" ? TrangThaiNV::TAM_NGHI : TrangThaiNV::DANG_LAM);
        onLuuDuLieu();
        refreshDashboard();
        refreshTableNhanVien();
    }
}

void MainWindow::onChoNghiViec() {
    int row = tableNhanVien->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 nhan vien!");
        return;
    }
    std::string maNV = tableNhanVien->item(row, 1)->text().toStdString();
    if (QMessageBox::question(this, "Xac nhan", QString("Ban co chac muon cho nhan vien %1 nghi viec?").arg(QString::fromStdString(maNV))) == QMessageBox::Yes) {
        app.xoaNhanVien(maNV);
        onLuuDuLieu();
        refreshDashboard();
        refreshTableNhanVien();
    }
}

void MainWindow::onTaoCa() {
    std::string ngay = dateLich->date().toString("dd/MM/yyyy").toStdString();
    std::string maCa = editMaCa->text().trimmed().toStdString();
    std::string tenCa = editTenCa->text().trimmed().toStdString();
    if (tenCa.empty()) tenCa = "Ca bo sung";
    std::string bd = editGioBatDau->text().trimmed().toStdString();
    std::string kt = editGioKetThuc->text().trimmed().toStdString();

    if (maCa.empty() || !Utils::isValidGio(bd) || !Utils::isValidGio(kt)) {
        QMessageBox::warning(this, "Loi", "Vui long nhap Ma ca va gio hop le theo dinh dang HH:MM (VD: 06:00, 09:00)!");
        return;
    }

    if (maCa == "CA_SANG" || maCa == "CA_CHIEU" || maCa == "CA_TOI") {
        QMessageBox::warning(this, "Loi", "Ma ca nay trung voi ma ca co dinh! Vui long dat ma khac (VD: CA_BS01).");
        return;
    }

    if (app.layLich().timCa(ngay, maCa)) {
        QMessageBox::warning(this, "Loi", QString("Ma ca %1 da ton tai trong ngay %2!").arg(QString::fromStdString(maCa), QString::fromStdString(ngay)));
        return;
    }

    app.layLich().themCa(CaLamViec(maCa, ngay, tenCa, bd, kt));
    editMaCa->clear();
    editTenCa->clear();
    editGioBatDau->clear();
    editGioKetThuc->clear();
    onLuuDuLieu();
    refreshDashboard();
    refreshTableLichNgay();
    QMessageBox::information(this, "Thanh cong", "Da tao ca lam viec bo sung moi!");
}

void MainWindow::onXepCa() {
    int row = tableLichNgay->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 ca trong bang de chi dinh nhan vien!");
        return;
    }
    std::string ngay = dateLich->date().toString("dd/MM/yyyy").toStdString();
    std::string maCa = tableLichNgay->item(row, 0)->text().toStdString();

    if (!comboNVXepCa || comboNVXepCa->currentIndex() < 0) {
        QMessageBox::warning(this, "Thong bao", "Khong co nhan vien nao de chi dinh!");
        return;
    }
    std::string maNV = comboNVXepCa->currentData().toString().toStdString();
    if (maNV.empty()) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon nhan vien hop le!");
        return;
    }

    CaLamViec* caHienTai = app.layLich().timCa(ngay, maCa);
    if (caHienTai && caHienTai->coNhanVien(maNV)) {
        QMessageBox::warning(this, "Thong bao", QString("Nhan vien %1 da co trong ca nay roi!").arg(QString::fromStdString(maNV)));
        return;
    }

    if (caHienTai && app.layLich().bTrungCa(ngay, maNV, caHienTai->getGioBatDau(), caHienTai->getGioKetThuc())) {
        QMessageBox::warning(this, "Trung gio lam viec",
            QString("Nhan vien %1 da duoc xep ca khac co khung gio trung hoac chong cheo trong ngay %2!")
            .arg(QString::fromStdString(maNV), QString::fromStdString(ngay)));
        return;
    }

    if (app.xepCa(ngay, maCa, maNV)) {
        onLuuDuLieu();
        refreshDashboard();
        refreshTableLichNgay();
        QMessageBox::information(this, "Thanh cong",
            QString("Da chi dinh NV %1 vao ca %2 ngay %3 thanh cong!\n(1 NV co the lam tat ca cac ca trong ngay neu khong trung gio)")
            .arg(QString::fromStdString(maNV), QString::fromStdString(maCa), QString::fromStdString(ngay)));
    } else {
        QMessageBox::warning(this, "That bai", "Khong the xep ca cho nhan vien nay!");
    }
}

void MainWindow::onXoaNhanVienKhoiCa() {
    int row = tableLichNgay->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 ca trong bang!");
        return;
    }
    std::string ngay = dateLich->date().toString("dd/MM/yyyy").toStdString();
    std::string maCa = tableLichNgay->item(row, 0)->text().toStdString();

    CaLamViec* ca = app.layLich().timCa(ngay, maCa);
    if (!ca || ca->getDsNhanVien().empty()) {
        QMessageBox::warning(this, "Thong bao", "Ca nay hien chua co nhan vien nao duoc phan cong!");
        return;
    }

    QStringList items;
    for (const auto& m : ca->getDsNhanVien()) {
        NhanVien* emp = app.timNhanVien(m);
        if (emp) {
            items << QString("%1 - %2").arg(QString::fromStdString(m), QString::fromStdString(emp->getHoTen()));
        } else {
            items << QString::fromStdString(m);
        }
    }

    bool ok = false;
    QString selected = QInputDialog::getItem(this, "Xoa Nhan Vien Khoi Ca", "Chon nhan vien can xoa:", items, 0, false, &ok);
    if (ok && !selected.isEmpty()) {
        std::string maNV = selected.split(" - ").first().trimmed().toStdString();
        if (app.layLich().boNhanVien(ngay, maCa, maNV)) {
            onLuuDuLieu();
            refreshDashboard();
            refreshTableLichNgay();
            QMessageBox::information(this, "Thanh cong", QString("Da xoa nhan vien %1 khoi ca %2!").arg(QString::fromStdString(maNV), QString::fromStdString(maCa)));
        } else {
            QMessageBox::warning(this, "That bai", "Khong the xoa nhan vien khoi ca!");
        }
    }
}

void MainWindow::onXoaCaBoSung() {
    int row = tableLichNgay->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon ca bo sung can xoa trong bang!");
        return;
    }
    std::string ngay = dateLich->date().toString("dd/MM/yyyy").toStdString();
    std::string maCa = tableLichNgay->item(row, 0)->text().toStdString();

    if (maCa == "CA_SANG" || maCa == "CA_CHIEU" || maCa == "CA_TOI") {
        QMessageBox::warning(this, "Khong the xoa", "Day la ca co dinh hang ngay cua quan, khong the xoa!\nNeu khong co ai lam ca nay, ban chi can khong phan cong nhan vien.");
        return;
    }

    if (QMessageBox::question(this, "Xac nhan xoa ca", QString("Ban co chac chan muon xoa ca bo sung [%1] khong?").arg(QString::fromStdString(maCa))) == QMessageBox::Yes) {
        if (app.layLich().xoaCa(ngay, maCa)) {
            onLuuDuLieu();
            refreshDashboard();
            refreshTableLichNgay();
            QMessageBox::information(this, "Thanh cong", QString("Da xoa ca bo sung %1!").arg(QString::fromStdString(maCa)));
        } else {
            QMessageBox::warning(this, "That bai", "Khong the xoa ca lam viec!");
        }
    }
}

void MainWindow::onAdminChamCongVao() {
    std::string maNV = editMaNVChamCong->text().trimmed().toStdString();
    if (maNV.empty()) {
        QMessageBox::warning(this, "Thong bao", "Vui long nhap Ma nhan vien can check-in!");
        return;
    }

    auto kq = app.kiemTraVaChamCongVao(maNV);
    if (kq.thanhCong) {
        onLuuDuLieu();
        refreshDashboard();
        refreshTableChamCong();
        QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq.thongBao));
    } else {
        QMessageBox::warning(this, "Khong the check-in", QString::fromStdString(kq.thongBao));
    }
}

void MainWindow::onAdminChamCongRa() {
    std::string maNV = editMaNVChamCong->text().trimmed().toStdString();
    if (maNV.empty()) {
        QMessageBox::warning(this, "Thong bao", "Vui long nhap Ma nhan vien can check-out!");
        return;
    }

    auto kq = app.kiemTraVaChamCongRa(maNV, "", false);
    if (kq.thanhCong) {
        onLuuDuLieu();
        refreshDashboard();
        refreshTableChamCong();
        QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq.thongBao));
    } else if (kq.codeLoi == 2) {
        // Chưa đến giờ kết thúc ca quy định
        int ans = QMessageBox::question(this, "Chua den gio quy dinh",
            QString::fromStdString(kq.thongBao) + "\n\nBan co muon xac nhan Check-out VE SOM cho NV nay khong?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            auto kq2 = app.kiemTraVaChamCongRa(maNV, "", true);
            if (kq2.thanhCong) {
                onLuuDuLieu();
                refreshDashboard();
                refreshTableChamCong();
                QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq2.thongBao));
            }
        }
    } else {
        QMessageBox::warning(this, "Khong the check-out", QString::fromStdString(kq.thongBao));
    }
}

void MainWindow::onTinhLuongThang() {
    refreshTableLuong();
    onLuuDuLieu();
    QMessageBox::information(this, "Thanh cong", "Da tinh toan va cap nhat bang luong thang!");
}

void MainWindow::onThemTaiKhoan() {
    bool ok = false;
    QString u = QInputDialog::getText(this, "Them Tai Khoan", "Ten dang nhap:", QLineEdit::Normal, "", &ok);
    if (!ok || u.trimmed().isEmpty()) return;

    QString p = QInputDialog::getText(this, "Them Tai Khoan", "Mat khau:", QLineEdit::Password, "", &ok);
    if (!ok || p.trimmed().isEmpty()) return;

    QStringList roles = {"EMPLOYEE", "ADMIN"};
    QString r = QInputDialog::getItem(this, "Them Tai Khoan", "Vai tro:", roles, 0, false, &ok);
    if (!ok) return;

    QString maNV = "";
    if (r == "EMPLOYEE") {
        maNV = QInputDialog::getText(this, "Lien Ket NV", "Ma nhan vien lien ket (VD: NV1):", QLineEdit::Normal, "", &ok);
    }

    if (qlTK.themTaiKhoan(u.trimmed().toStdString(), p.trimmed().toStdString(), r == "ADMIN" ? VaiTro::ADMIN : VaiTro::EMPLOYEE, maNV.trimmed().toStdString())) {
        onLuuDuLieu();
        refreshTableTaiKhoan();
        QMessageBox::information(this, "Thanh cong", "Da them tai khoan moi!");
    } else {
        QMessageBox::warning(this, "That bai", "Ten dang nhap da ton tai!");
    }
}

void MainWindow::onResetMatKhau() {
    int row = tableTaiKhoan->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 tai khoan trong bang!");
        return;
    }
    std::string user = tableTaiKhoan->item(row, 0)->text().toStdString();
    if (qlTK.resetMatKhau(user)) {
        onLuuDuLieu();
        QMessageBox::information(this, "Thanh cong", QString("Da reset mat khau cua %1 ve 123456!").arg(QString::fromStdString(user)));
    }
}

void MainWindow::onXoaTaiKhoan() {
    int row = tableTaiKhoan->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Thong bao", "Vui long chon 1 tai khoan trong bang!");
        return;
    }
    std::string user = tableTaiKhoan->item(row, 0)->text().toStdString();
    if (user == qlTK.getTenDangNhapHienTai()) {
        QMessageBox::warning(this, "Loi", "Khong the xoa tai khoan dang dang nhap!");
        return;
    }
    if (QMessageBox::question(this, "Xac nhan", QString("Ban co chac muon xoa tai khoan %1?").arg(QString::fromStdString(user))) == QMessageBox::Yes) {
        if (qlTK.xoaTaiKhoan(user)) {
            onLuuDuLieu();
            refreshTableTaiKhoan();
        }
    }
}

// =========================================================================
// EMPLOYEE SLOTS
// =========================================================================
void MainWindow::onEmpDoiAvatar() {
    std::string maNV = qlTK.getMaNVHienTai();
    NhanVien* nv = app.timNhanVien(maNV);
    if (!nv) return;

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Chon anh dai dien ca nhan",
        "",
        "Hinh anh (*.png *.jpg *.jpeg *.bmp)"
    );

    if (!filePath.isEmpty()) {
        QString relPath = AvatarHelper::saveAvatarImage(filePath, maNV, thuMucDuLieu);
        if (!relPath.isEmpty()) {
            nv->setAvatar(relPath.toStdString());
            onLuuDuLieu();
            refreshEmployeeInfo();
            QMessageBox::information(this, "Thanh cong", "Da cap nhat anh dai dien ca nhan thanh cong!");
        }
    }
}

void MainWindow::onEmpCapNhatLienHe() {
    std::string maNV = qlTK.getMaNVHienTai();
    NhanVien* nv = app.timNhanVien(maNV);
    if (!nv) return;

    std::string sdt = editEmpSDT->text().trimmed().toStdString();
    std::string email = editEmpEmail->text().trimmed().toStdString();

    if (!Utils::isValidSDT(sdt)) {
        QMessageBox::warning(this, "Loi", "So dien thoai khong hop le!");
        return;
    }
    if (!Utils::isValidEmail(email)) {
        QMessageBox::warning(this, "Loi", "Email khong hop le!");
        return;
    }

    nv->setSdt(sdt);
    nv->setEmail(email);
    onLuuDuLieu();
    QMessageBox::information(this, "Thanh cong", "Da cap nhat thong tin lien he ca nhan!");
}

void MainWindow::onEmpDoiMatKhau() {
    std::string cu = editEmpPassCu->text().trimmed().toStdString();
    std::string moi = editEmpPassMoi->text().trimmed().toStdString();
    std::string xn = editEmpPassXacNhan->text().trimmed().toStdString();

    TaiKhoan* tk = qlTK.timTaiKhoan(qlTK.getTenDangNhapHienTai());
    if (!tk || !tk->kiemTraMatKhau(cu)) {
        QMessageBox::warning(this, "Loi", "Mat khau cu khong chinh xac!");
        return;
    }
    if (moi != xn) {
        QMessageBox::warning(this, "Loi", "Xac nhan mat khau moi khong khop!");
        return;
    }
    if (moi.length() < 4) {
        QMessageBox::warning(this, "Loi", "Mat khau moi phai co it nhat 4 ky tu!");
        return;
    }

    tk->doiMatKhau(moi);
    editEmpPassCu->clear();
    editEmpPassMoi->clear();
    editEmpPassXacNhan->clear();
    onLuuDuLieu();
    QMessageBox::information(this, "Thanh cong", "Doi mat khau thanh cong!");
}

void MainWindow::onEmpChamCongVao() {
    std::string maNV = qlTK.getMaNVHienTai();
    auto kq = app.kiemTraVaChamCongVao(maNV);
    if (kq.thanhCong) {
        onLuuDuLieu();
        refreshEmployeeChamCong();
        QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq.thongBao));
    } else {
        QMessageBox::warning(this, "Thong bao cham cong", QString::fromStdString(kq.thongBao));
    }
}

void MainWindow::onEmpChamCongRa() {
    std::string maNV = qlTK.getMaNVHienTai();
    auto kq = app.kiemTraVaChamCongRa(maNV, "", false);
    if (kq.thanhCong) {
        onLuuDuLieu();
        refreshEmployeeChamCong();
        refreshEmployeeLuong();
        QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq.thongBao));
    } else if (kq.codeLoi == 2) {
        // Chưa đến giờ kết thúc ca quy định
        int ans = QMessageBox::question(this, "Chua den gio tan ca quy dinh",
            QString::fromStdString(kq.thongBao) + "\n\nBan co chac chan muon xac nhan Check-out VE SOM khong?\n(Trang thai se duoc ghi nhan la 'Ve som')",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ans == QMessageBox::Yes) {
            auto kq2 = app.kiemTraVaChamCongRa(maNV, "", true);
            if (kq2.thanhCong) {
                onLuuDuLieu();
                refreshEmployeeChamCong();
                refreshEmployeeLuong();
                QMessageBox::information(this, "Thanh cong", QString::fromStdString(kq2.thongBao));
            }
        }
    } else {
        QMessageBox::warning(this, "Khong the check-out", QString::fromStdString(kq.thongBao));
    }
}

void MainWindow::onEmpThangLuongChanged() {
    refreshEmployeeLuong();
}

void MainWindow::onLuuDuLieu() {
    app.layLich().chuanHoaLich();
    app.luuTatCa(thuMucDuLieu);
    qlTK.luuFile(thuMucDuLieu + "/taikhoan.csv");
    statusBar()->showMessage("Da luu du lieu an toan xuong dia!", 3000);
}

void MainWindow::onLuuDuLieuLich() {
    onLuuDuLieu();
    refreshDashboard();
    refreshTableLichNgay();
    QMessageBox::information(this, "Thanh Cong",
        "Da luu lich lam viec thanh cong!\nDu lieu duoc dong bo va chuan hoa an toan, khong bi trung lap.");
}

void MainWindow::onDangXuat() {
    onLuuDuLieu();
    qlTK.dangXuat();
    close(); // Đóng MainWindow để main_gui mở lại LoginDialog
}

// =========================================================================
// GIAO DIEN ORDER & BAN (iPOS STYLE)
// =========================================================================
QWidget* MainWindow::createOrderWidget() {
    QWidget* tabOrder = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(tabOrder);
    mainLayout->setSpacing(12);

    // ----------------- CỘT 1: SƠ ĐỒ 30 BÀN -----------------
    QGroupBox* boxBan = new QGroupBox("So Do Ban (30 Ban)");
    boxBan->setStyleSheet("QGroupBox { font-weight: bold; font-size: 13px; }");
    QVBoxLayout* vbBan = new QVBoxLayout(boxBan);

    // Chú thích màu sắc
    QHBoxLayout* hbChuThich = new QHBoxLayout();
    QLabel* lblColorBlue = new QLabel("■ Da co mon (Xanh)");
    lblColorBlue->setStyleSheet("color: #0288D1; font-weight: bold; font-size: 11px;");
    QLabel* lblColorGray = new QLabel("■ Ban trong (Xam)");
    lblColorGray->setStyleSheet("color: #757575; font-weight: bold; font-size: 11px;");
    hbChuThich->addWidget(lblColorBlue);
    hbChuThich->addWidget(lblColorGray);
    vbBan->addLayout(hbChuThich);

    QScrollArea* scrollBan = new QScrollArea();
    scrollBan->setWidgetResizable(true);
    QWidget* widgetBanGrid = new QWidget();
    QGridLayout* gridBan = new QGridLayout(widgetBanGrid);
    gridBan->setSpacing(8);

    for (int i = 0; i < 30; ++i) {
        int soBan = i + 1;
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(78, 62);
        btnDanhSachBan[i] = btn;

        connect(btn, &QPushButton::clicked, this, [this, soBan]() {
            onChonBan(soBan);
        });

        int row = i / 5;
        int col = i % 5;
        gridBan->addWidget(btn, row, col);
    }

    scrollBan->setWidget(widgetBanGrid);
    vbBan->addWidget(scrollBan);
    mainLayout->addWidget(boxBan, 4);

    // ----------------- CỘT 2: CHI TIẾT ORDER & HÓA ĐƠN -----------------
    QGroupBox* boxChiTiet = new QGroupBox("Chi Tiet Order");
    boxChiTiet->setStyleSheet("QGroupBox { font-weight: bold; font-size: 13px; }");
    QVBoxLayout* vbChiTiet = new QVBoxLayout(boxChiTiet);

    lblThongTinBan = new QLabel("Dang chon: BAN 1 (TRONG)");
    lblThongTinBan->setStyleSheet("font-size: 14px; font-weight: bold; color: #1565C0; margin-bottom: 4px;");
    vbChiTiet->addWidget(lblThongTinBan);

    tableChiTietOrder = new QTableWidget();
    tableChiTietOrder->setColumnCount(6);
    tableChiTietOrder->setHorizontalHeaderLabels({"STT", "Ten Mon", "Don Gia", "SL", "Thanh Tien", "Trang Thai"});
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableChiTietOrder->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tableChiTietOrder->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableChiTietOrder->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vbChiTiet->addWidget(tableChiTietOrder);

    // Các nút điều chỉnh số lượng món
    QHBoxLayout* hbDieuChinh = new QHBoxLayout();
    QPushButton* btnGiam = new QPushButton("-1 So Luong");
    btnGiam->setStyleSheet("background-color: #E0E0E0; font-weight: bold; height: 32px; border-radius: 4px;");
    QPushButton* btnTang = new QPushButton("+1 So Luong");
    btnTang->setStyleSheet("background-color: #E0E0E0; font-weight: bold; height: 32px; border-radius: 4px;");
    QPushButton* btnXoaMon = new QPushButton("Xoa Mon Nay");
    btnXoaMon->setStyleSheet("background-color: #FFCDD2; color: #C62828; font-weight: bold; height: 32px; border-radius: 4px;");

    connect(btnGiam, &QPushButton::clicked, this, [this]() { onSuaSoLuongMon(-1); });
    connect(btnTang, &QPushButton::clicked, this, [this]() { onSuaSoLuongMon(1); });
    connect(btnXoaMon, &QPushButton::clicked, this, &MainWindow::onXoaMonKhoiBan);

    hbDieuChinh->addWidget(btnGiam);
    hbDieuChinh->addWidget(btnTang);
    hbDieuChinh->addWidget(btnXoaMon);
    vbChiTiet->addLayout(hbDieuChinh);

    // Tổng tiền & số lượng
    QFrame* frameSummary = new QFrame();
    frameSummary->setStyleSheet("background-color: #F5F5F5; border: 1px solid #E0E0E0; border-radius: 6px; padding: 6px;");
    QVBoxLayout* vbSummary = new QVBoxLayout(frameSummary);
    lblTongSoMonOrder = new QLabel("Tong so luong: 0 mon");
    lblTongSoMonOrder->setStyleSheet("font-size: 13px; font-weight: bold; color: #424242;");
    lblTongTienOrder = new QLabel("TONG TIEN: 0 VND");
    lblTongTienOrder->setStyleSheet("font-size: 18px; font-weight: bold; color: #2E7D32;");
    vbSummary->addWidget(lblTongSoMonOrder);
    vbSummary->addWidget(lblTongTienOrder);
    vbChiTiet->addWidget(frameSummary);

    // Các nút chốt bill / thanh toán
    QHBoxLayout* hbActions = new QHBoxLayout();
    QPushButton* btnChotBill = new QPushButton("Chot Bill / Gui Pha Che");
    btnChotBill->setStyleSheet("background-color: #1976D2; color: white; font-weight: bold; font-size: 13px; height: 40px; border-radius: 5px;");
    QPushButton* btnThanhToan = new QPushButton("Thanh Toan");
    btnThanhToan->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; font-size: 13px; height: 40px; border-radius: 5px;");
    QPushButton* btnHuy = new QPushButton("Tra Ban");
    btnHuy->setStyleSheet("background-color: #757575; color: white; font-weight: bold; font-size: 12px; height: 40px; border-radius: 5px;");

    connect(btnChotBill, &QPushButton::clicked, this, &MainWindow::onChotOrder);
    connect(btnThanhToan, &QPushButton::clicked, this, &MainWindow::onThanhToanBan);
    connect(btnHuy, &QPushButton::clicked, this, &MainWindow::onHuyBan);

    hbActions->addWidget(btnChotBill, 2);
    hbActions->addWidget(btnThanhToan, 2);
    hbActions->addWidget(btnHuy, 1);
    vbChiTiet->addLayout(hbActions);

    mainLayout->addWidget(boxChiTiet, 5);

    // ----------------- CỘT 3: THỰC ĐƠN (MENU 8 MÓN) -----------------
    QGroupBox* boxMenu = new QGroupBox("Thuc Don Quan (Menu)");
    boxMenu->setStyleSheet("QGroupBox { font-weight: bold; font-size: 13px; }");
    QVBoxLayout* vbMenu = new QVBoxLayout(boxMenu);

    QScrollArea* scrollMenu = new QScrollArea();
    scrollMenu->setWidgetResizable(true);
    QWidget* widgetMenuGrid = new QWidget();
    QVBoxLayout* menuListLayout = new QVBoxLayout(widgetMenuGrid);
    menuListLayout->setSpacing(8);

    const auto& dsMenu = app.layQuanLyOrder().layMenu();
    for (const auto& item : dsMenu) {
        QFrame* card = new QFrame();
        card->setStyleSheet("QFrame { background-color: #FFFFFF; border: 1px solid #E0E0E0; border-radius: 8px; padding: 6px; } "
                            "QFrame:hover { border: 1px solid #1976D2; background-color: #F8FBFF; }");
        QHBoxLayout* cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(6, 6, 6, 6);

        // Ảnh món
        QLabel* lblImg = new QLabel();
        lblImg->setFixedSize(65, 65);
        lblImg->setStyleSheet("border: 1px solid #EEEEEE; border-radius: 6px; background-color: #FAF5EF;");
        lblImg->setAlignment(Qt::AlignCenter);

        QPixmap pm(QString::fromStdString(item.hinhAnh));
        if (!pm.isNull()) {
            lblImg->setPixmap(pm.scaled(63, 63, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            lblImg->setText("CF");
        }
        cardLayout->addWidget(lblImg);

        // Thông tin món
        QVBoxLayout* infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(2);
        QLabel* lblTen = new QLabel(QString::fromStdString(item.tenMon));
        lblTen->setStyleSheet("font-weight: bold; font-size: 13px; color: #212121;");
        QLabel* lblGia = new QLabel(QString("%1 VND").arg(QString::number(item.donGia, 'f', 0)));
        lblGia->setStyleSheet("font-weight: bold; font-size: 13px; color: #D32F2F;");
        infoLayout->addWidget(lblTen);
        infoLayout->addWidget(lblGia);
        cardLayout->addLayout(infoLayout, 1);

        // Nút thêm
        QPushButton* btnThem = new QPushButton("+ Them");
        btnThem->setStyleSheet("background-color: #E3F2FD; color: #1976D2; font-weight: bold; border-radius: 5px; height: 32px; padding: 0 10px;");
        std::string maM = item.maMon;
        connect(btnThem, &QPushButton::clicked, this, [this, maM]() {
            onThemMonVaoBan(QString::fromStdString(maM));
        });
        cardLayout->addWidget(btnThem);

        menuListLayout->addWidget(card);
    }
    menuListLayout->addStretch();

    scrollMenu->setWidget(widgetMenuGrid);
    vbMenu->addWidget(scrollMenu);
    mainLayout->addWidget(boxMenu, 4);

    // Cập nhật trạng thái bàn ban đầu
    refreshSoDoBan();
    refreshChiTietBan(banDangChon);

    return tabOrder;
}

// =========================================================================
// GIAO DIỆN QUẦY PHA CHẾ (KDS)
// =========================================================================
QWidget* MainWindow::createKDSWidget() {
    QWidget* tabKDS = new QWidget();
    QVBoxLayout* vlKDS = new QVBoxLayout(tabKDS);

    QHBoxLayout* hbHeader = new QHBoxLayout();
    lblKDSStats = new QLabel("Dang tai danh sach mon can pha che...");
    lblKDSStats->setStyleSheet("font-size: 15px; font-weight: bold; color: #0D47A1;");
    QPushButton* btnRefresh = new QPushButton("Lam Moi (F5)");
    btnRefresh->setStyleSheet("background-color: #1976D2; color: white; font-weight: bold; height: 35px; padding: 0 15px; border-radius: 4px;");
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshKDS);

    hbHeader->addWidget(lblKDSStats, 1);
    hbHeader->addWidget(btnRefresh);
    vlKDS->addLayout(hbHeader);

    tableKDS = new QTableWidget();
    tableKDS->setColumnCount(8);
    tableKDS->setHorizontalHeaderLabels({"STT", "Ban", "Hinh Anh", "Ten Mon", "So Luong", "Gio Dat", "Trang Thai", "Thao Tac"});
    tableKDS->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    tableKDS->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    tableKDS->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    tableKDS->verticalHeader()->setDefaultSectionSize(65);
    tableKDS->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlKDS->addWidget(tableKDS);

    refreshKDS();
    return tabKDS;
}

// =========================================================================
// GIAO DIỆN MÓN CẦN BƯNG (DÀNH CHO PHỤC VỤ)
// =========================================================================
QWidget* MainWindow::createCanBungWidget() {
    QWidget* tabCanBung = new QWidget();
    QVBoxLayout* vlCanBung = new QVBoxLayout(tabCanBung);

    QHBoxLayout* hbHeader = new QHBoxLayout();
    lblCanBungStats = new QLabel("Cac mon da pha che xong - San sang bung ra ban:");
    lblCanBungStats->setStyleSheet("font-size: 15px; font-weight: bold; color: #2E7D32;");
    QPushButton* btnRefresh = new QPushButton("Lam Moi");
    btnRefresh->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; height: 35px; padding: 0 15px; border-radius: 4px;");
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshCanBung);

    hbHeader->addWidget(lblCanBungStats, 1);
    hbHeader->addWidget(btnRefresh);
    vlCanBung->addLayout(hbHeader);

    tableCanBung = new QTableWidget();
    tableCanBung->setColumnCount(7);
    tableCanBung->setHorizontalHeaderLabels({"STT", "Ban", "Ten Mon", "So Luong", "Gio Dat", "Trang Thai", "Thao Tac"});
    tableCanBung->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableCanBung->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableCanBung->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableCanBung->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableCanBung->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableCanBung->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tableCanBung->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    tableCanBung->setEditTriggers(QAbstractItemView::NoEditTriggers);
    vlCanBung->addWidget(tableCanBung);

    refreshCanBung();
    return tabCanBung;
}

// =========================================================================
// LOGIC REFRESH & XỬ LÝ SỰ KIỆN ORDER / KDS / BƯNG MÓN
// =========================================================================
void MainWindow::refreshSoDoBan() {
    if (!btnDanhSachBan[0]) return;
    auto& qlO = app.layQuanLyOrder();

    for (int i = 0; i < 30; ++i) {
        int soBan = i + 1;
        QPushButton* btn = btnDanhSachBan[i];
        if (!btn) continue;

        const BanAn* b = qlO.layBan(soBan);
        bool coMon = (b && b->coNguoi() && !b->getDsMon().empty());

        QString text = QString("BAN %1\n").arg(soBan);
        QString style;

        if (coMon) {
            text += QString("(%1 mon)").arg(b->tongSoLuongMon());
            if (soBan == banDangChon) {
                style = "background-color: #0288D1; color: white; border: 3px solid #FF9800; border-radius: 8px; font-weight: bold; font-size: 12px;";
            } else {
                style = "background-color: #0288D1; color: white; border: 2px solid #01579B; border-radius: 8px; font-weight: bold; font-size: 12px;";
            }
        } else {
            text += "(Trong)";
            if (soBan == banDangChon) {
                style = "background-color: #E0E0E0; color: #424242; border: 3px solid #FF9800; border-radius: 8px; font-weight: bold; font-size: 12px;";
            } else {
                style = "background-color: #E0E0E0; color: #616161; border: 1px solid #BDBDBD; border-radius: 8px; font-weight: bold; font-size: 12px;";
            }
        }

        btn->setText(text);
        btn->setStyleSheet(style);
    }
}

void MainWindow::refreshChiTietBan(int soBan) {
    if (!tableChiTietOrder || !lblThongTinBan) return;
    auto& qlO = app.layQuanLyOrder();
    const BanAn* b = qlO.layBan(soBan);

    if (!b || !b->coNguoi() || b->getDsMon().empty()) {
        lblThongTinBan->setText(QString("Dang chon: BAN %1 (TRONG)").arg(soBan));
        tableChiTietOrder->setRowCount(0);
        lblTongSoMonOrder->setText("Tong so luong: 0 mon");
        lblTongTienOrder->setText("TONG TIEN: 0 VND");
        return;
    }

    lblThongTinBan->setText(QString("Dang chon: BAN %1 (DA CO KHACH - Vao luc: %2)")
        .arg(soBan).arg(QString::fromStdString(b->getThoiGianVao())));

    const auto& dsMon = b->getDsMon();
    tableChiTietOrder->setRowCount(dsMon.size());

    for (size_t i = 0; i < dsMon.size(); ++i) {
        const auto& m = dsMon[i];
        tableChiTietOrder->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tableChiTietOrder->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m.tenMon)));
        tableChiTietOrder->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(QString::number(m.donGia, 'f', 0))));
        tableChiTietOrder->setItem(i, 3, new QTableWidgetItem(QString::number(m.soLuong)));
        tableChiTietOrder->setItem(i, 4, new QTableWidgetItem(QString("%1").arg(QString::number(m.tinhThanhTien(), 'f', 0))));

        QString ttText = "Cho lam";
        if (m.trangThai == TrangThaiMonOrder::DANG_LAM) ttText = "Dang pha che";
        else if (m.trangThai == TrangThaiMonOrder::DA_XONG) ttText = "Da xong (Cho bung)";
        else if (m.trangThai == TrangThaiMonOrder::DA_PHUC_VU) ttText = "Da phuc vu";

        QTableWidgetItem* itTrangThai = new QTableWidgetItem(ttText);
        if (m.trangThai == TrangThaiMonOrder::CHO_LAM) itTrangThai->setForeground(QBrush(QColor("#E65100")));
        else if (m.trangThai == TrangThaiMonOrder::DANG_LAM) itTrangThai->setForeground(QBrush(QColor("#1565C0")));
        else if (m.trangThai == TrangThaiMonOrder::DA_XONG) itTrangThai->setForeground(QBrush(QColor("#2E7D32")));
        tableChiTietOrder->setItem(i, 5, itTrangThai);
    }

    lblTongSoMonOrder->setText(QString("Tong so luong: %1 mon").arg(b->tongSoLuongMon()));
    lblTongTienOrder->setText(QString("TONG TIEN: %1 VND").arg(QString::number(b->tinhTongTien(), 'f', 0)));
}

void MainWindow::onChonBan(int soBan) {
    banDangChon = soBan;
    refreshSoDoBan();
    refreshChiTietBan(banDangChon);
}

void MainWindow::onThemMonVaoBan(const QString& maMon) {
    auto& qlO = app.layQuanLyOrder();
    qlO.datMon(banDangChon, maMon.toStdString(), 1, Utils::gioHienTai());
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshSoDoBan();
    refreshChiTietBan(banDangChon);
    statusBar()->showMessage(QString("Da them mon vao Ban %1").arg(banDangChon), 2000);
}

void MainWindow::onSuaSoLuongMon(int delta) {
    int row = tableChiTietOrder ? tableChiTietOrder->currentRow() : -1;
    if (row < 0) {
        QMessageBox::warning(this, "Chua chon mon", "Vui long chon mon can sua so luong trong bang!");
        return;
    }

    auto& qlO = app.layQuanLyOrder();
    BanAn* b = qlO.layBan(banDangChon);
    if (!b || row >= (int)b->getDsMon().size()) return;

    std::string maM = b->getDsMon()[row].maMon;
    int slMoi = b->getDsMon()[row].soLuong + delta;

    qlO.suaMon(banDangChon, maM, slMoi);
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshSoDoBan();
    refreshChiTietBan(banDangChon);
}

void MainWindow::onXoaMonKhoiBan() {
    int row = tableChiTietOrder ? tableChiTietOrder->currentRow() : -1;
    if (row < 0) {
        QMessageBox::warning(this, "Chua chon mon", "Vui long chon mon can xoa trong bang!");
        return;
    }

    auto& qlO = app.layQuanLyOrder();
    BanAn* b = qlO.layBan(banDangChon);
    if (!b || row >= (int)b->getDsMon().size()) return;

    std::string maM = b->getDsMon()[row].maMon;
    qlO.xoaMon(banDangChon, maM);
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshSoDoBan();
    refreshChiTietBan(banDangChon);
}

void MainWindow::onChotOrder() {
    auto& qlO = app.layQuanLyOrder();
    BanAn* b = qlO.layBan(banDangChon);
    if (!b || b->getDsMon().empty()) {
        QMessageBox::warning(this, "Ban trong", "Ban chua co mon nao de chot bill!");
        return;
    }

    b->setCoNguoi(true);
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshSoDoBan();
    refreshChiTietBan(banDangChon);
    QMessageBox::information(this, "Chot Order",
        QString("Da chot order cho Ban %1 thanh cong!\nDanh sach mon da duoc chuyen sang quay Pha Che.")
        .arg(banDangChon));
}

void MainWindow::onThanhToanBan() {
    auto& qlO = app.layQuanLyOrder();
    BanAn* b = qlO.layBan(banDangChon);
    if (!b || !b->coNguoi() || b->getDsMon().empty()) {
        QMessageBox::warning(this, "Khong the thanh toan", "Ban nay dang trong, khong co hoa don de thanh toan!");
        return;
    }

    double tong = b->tinhTongTien();
    QString xacNhan = QString("Xac nhan thanh toan cho BAN %1\n"
                              "Tong tien: %2 VND\n\n"
                              "Sau khi thanh toan, khach da ve va ban se chuyen lai mau xam.")
                      .arg(banDangChon).arg(QString::number(tong, 'f', 0));

    if (QMessageBox::question(this, "Thanh Toan", xacNhan, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        qlO.thanhToan(banDangChon, qlTK.getTenDangNhapHienTai(), thuMucDuLieu + "/hoadon.csv");
        qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

        refreshSoDoBan();
        refreshChiTietBan(banDangChon);
        QMessageBox::information(this, "Hoan Tat Thanh Toan",
            QString("Da thanh toan thanh cong cho Ban %1! Ban da ve trang thai trong (mau xam).")
            .arg(banDangChon));
    }
}

void MainWindow::onHuyBan() {
    auto& qlO = app.layQuanLyOrder();
    BanAn* b = qlO.layBan(banDangChon);
    if (!b || !b->coNguoi()) {
        QMessageBox::information(this, "Thong bao", "Ban nay von da trong.");
        return;
    }

    if (QMessageBox::question(this, "Tra ban",
        QString("Ban co chac muon huy toan bo mon va tra Ban %1 ve trang thai trong?").arg(banDangChon),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        b->resetBan();
        qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");
        refreshSoDoBan();
        refreshChiTietBan(banDangChon);
    }
}

void MainWindow::refreshKDS() {
    if (!tableKDS) return;
    auto& qlO = app.layQuanLyOrder();
    auto dsPhaChe = qlO.layDanhSachCanPhaChe();

    tableKDS->setRowCount(dsPhaChe.size());
    int choLam = 0, dangLam = 0;

    for (size_t i = 0; i < dsPhaChe.size(); ++i) {
        int sb = dsPhaChe[i].soBan;
        const auto& item = dsPhaChe[i].chiTiet;

        if (item.trangThai == TrangThaiMonOrder::CHO_LAM) choLam++;
        else if (item.trangThai == TrangThaiMonOrder::DANG_LAM) dangLam++;

        tableKDS->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tableKDS->setItem(i, 1, new QTableWidgetItem(QString("Ban %1").arg(sb)));

        // Hình ảnh món
        QLabel* lblImg = new QLabel();
        lblImg->setAlignment(Qt::AlignCenter);
        const MonAn* mon = qlO.timMon(item.maMon);
        if (mon) {
            QPixmap pm(QString::fromStdString(mon->hinhAnh));
            if (!pm.isNull()) lblImg->setPixmap(pm.scaled(55, 55, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        tableKDS->setCellWidget(i, 2, lblImg);

        tableKDS->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(item.tenMon)));
        tableKDS->setItem(i, 4, new QTableWidgetItem(QString::number(item.soLuong)));
        tableKDS->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(item.thoiGianDat)));

        QString tt = (item.trangThai == TrangThaiMonOrder::CHO_LAM) ? "Cho lam" : "Dang pha che";
        QTableWidgetItem* itTT = new QTableWidgetItem(tt);
        itTT->setForeground(QBrush(item.trangThai == TrangThaiMonOrder::CHO_LAM ? QColor("#E65100") : QColor("#1565C0")));
        tableKDS->setItem(i, 6, itTT);

        // Nút thao tác
        QWidget* pWidget = new QWidget();
        QHBoxLayout* hl = new QHBoxLayout(pWidget);
        hl->setContentsMargins(4, 4, 4, 4);

        std::string maM = item.maMon;
        if (item.trangThai == TrangThaiMonOrder::CHO_LAM) {
            QPushButton* btnXacNhan = new QPushButton("Xac Nhan");
            btnXacNhan->setStyleSheet("background-color: #E65100; color: white; font-weight: bold; border-radius: 4px; height: 32px; padding: 0 10px;");
            connect(btnXacNhan, &QPushButton::clicked, this, [this, sb, maM]() {
                onKDSXacNhan(sb, QString::fromStdString(maM));
            });
            hl->addWidget(btnXacNhan);
        } else {
            QPushButton* btnHoanThanh = new QPushButton("Hoan Thanh");
            btnHoanThanh->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; border-radius: 4px; height: 32px; padding: 0 10px;");
            connect(btnHoanThanh, &QPushButton::clicked, this, [this, sb, maM]() {
                onKDSHoanThanh(sb, QString::fromStdString(maM));
            });
            hl->addWidget(btnHoanThanh);
        }

        tableKDS->setCellWidget(i, 7, pWidget);
    }

    if (lblKDSStats) {
        lblKDSStats->setText(QString("Quay Pha Che (KDS) - Cho lam: %1 mon | Dang lam: %2 mon")
            .arg(choLam).arg(dangLam));
    }
}

void MainWindow::onKDSXacNhan(int soBan, const QString& maMon) {
    auto& qlO = app.layQuanLyOrder();
    qlO.xacNhanPhaChe(soBan, maMon.toStdString());
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshKDS();
    refreshChiTietBan(banDangChon);
    statusBar()->showMessage(QString("Quay Pha Che da xac nhan lam mon cho Ban %1").arg(soBan), 3000);
}

void MainWindow::onKDSHoanThanh(int soBan, const QString& maMon) {
    auto& qlO = app.layQuanLyOrder();
    qlO.hoanThanhPhaChe(soBan, maMon.toStdString());
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshKDS();
    refreshChiTietBan(banDangChon);
    QMessageBox::information(this, "Hoan Thanh Mon",
        QString("Da hoan thanh mon cho Ban %1!\nThong bao da duoc gui den nhan vien Phuc Vu de bung mon.")
        .arg(soBan));
}

void MainWindow::refreshCanBung() {
    if (!tableCanBung) return;
    auto& qlO = app.layQuanLyOrder();
    auto dsCanBung = qlO.layDanhSachCanBung();

    tableCanBung->setRowCount(dsCanBung.size());
    for (size_t i = 0; i < dsCanBung.size(); ++i) {
        int sb = dsCanBung[i].soBan;
        const auto& item = dsCanBung[i].chiTiet;

        tableCanBung->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tableCanBung->setItem(i, 1, new QTableWidgetItem(QString("Ban %1").arg(sb)));
        tableCanBung->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(item.tenMon)));
        tableCanBung->setItem(i, 3, new QTableWidgetItem(QString::number(item.soLuong)));
        tableCanBung->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(item.thoiGianDat)));

        QTableWidgetItem* itTT = new QTableWidgetItem("Da xong - Cho bung ra ban");
        itTT->setForeground(QBrush(QColor("#2E7D32")));
        tableCanBung->setItem(i, 5, itTT);

        QPushButton* btnDaBung = new QPushButton("Da Bung Ra Ban");
        btnDaBung->setStyleSheet("background-color: #1976D2; color: white; font-weight: bold; border-radius: 4px; height: 30px; padding: 0 10px;");
        std::string maM = item.maMon;
        connect(btnDaBung, &QPushButton::clicked, this, [this, sb, maM]() {
            onPhuVuDaBung(sb, QString::fromStdString(maM));
        });
        tableCanBung->setCellWidget(i, 6, btnDaBung);
    }

    if (lblCanBungStats) {
        lblCanBungStats->setText(QString("Danh sach mon can bung ra ban: %1 mon").arg(dsCanBung.size()));
    }
}

void MainWindow::onPhuVuDaBung(int soBan, const QString& maMon) {
    auto& qlO = app.layQuanLyOrder();
    qlO.xacNhanDaBung(soBan, maMon.toStdString());
    qlO.luuTrangThaiBan(thuMucDuLieu + "/ban_order.csv");

    refreshCanBung();
    refreshChiTietBan(banDangChon);
    statusBar()->showMessage(QString("Da bung mon ra Ban %1").arg(soBan), 3000);
}

void MainWindow::onTimerSyncOrder() {
    auto& qlO = app.layQuanLyOrder();

    // 1. Kiểm tra thông báo cho Phục vụ
    if (qlO.coThongBaoMoi()) {
        auto dsTB = qlO.layVaXoaThongBaoPhuVu();
        QString fullMsg;
        for (const auto& tb : dsTB) {
            if (!fullMsg.isEmpty()) fullMsg += "\n";
            fullMsg += "🔔 " + QString::fromStdString(tb);
        }
        if (!fullMsg.isEmpty()) {
            QMessageBox::information(this, "Thong Bao Phuc Vu", fullMsg);
        }
        if (tableCanBung) {
            refreshCanBung();
        }
    }

    // 2. Cập nhật màu bàn định kỳ
    if (btnDanhSachBan[0]) {
        refreshSoDoBan();
    }
}
