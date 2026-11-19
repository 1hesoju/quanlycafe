#!/bin/bash
if [ -f "./quanlycafe_gui" ]; then
    ./quanlycafe_gui
else
    echo "Chua tim thay file quanlycafe_gui, dang tien hanh bien dich..."
    bash Build_GUI.sh
    ./quanlycafe_gui
fi
