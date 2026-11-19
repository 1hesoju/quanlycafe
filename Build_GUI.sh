#!/bin/bash
set -e

echo "==================================================="
echo "   DANG BIEN DICH UNG DUNG QUAN LY CAFE (LINUX)"
echo "==================================================="

# Kiem tra CMake
if ! command -v cmake &> /dev/null; then
    echo "[LOI] Khong tim thay CMake! Vui long cai dat: sudo apt install cmake"
    exit 1
fi

echo "[1/3] Cau hinh CMake..."
cmake -B build_gui -DCMAKE_BUILD_TYPE=Release

echo ""
echo "[2/3] Bien dich ma nguon..."
cmake --build build_gui -j$(nproc)

echo ""
echo "[3/3] Sao chep file thuc thi..."
cp -f build_gui/quanlycafe_gui ./quanlycafe_gui

echo ""
echo "==================================================="
echo "   BIEN DICH HOAN TAT THANH CONG!"
echo "   Da tao file: ./quanlycafe_gui"
echo "   Chay ung dung: ./quanlycafe_gui hoac ./Chay_App_GUI.sh"
echo "==================================================="
