#!/bin/sh
set -e
ver=$1
rel=release/MinimOSDx2-$ver
rm -rf $rel
mkdir -p $rel
cd osd
make clean
make OSD_LANG=OSD_RUSSIAN
mv build-atmega328/osd.hex ../$rel/MinimOSDx2_ru-$ver.hex
make clean
make
mv build-atmega328/osd.hex ../$rel/MinimOSDx2_en-$ver.hex
cd ..
cp config/minimosd_cfg $rel/
cp config/*.mcm $rel/
cp doc/*.pdf $rel/
cd $rel
cp minimosd_cfg minimosd_cfg.tcl
../../../freewrap/linux64/freewrapTCLSH -w ../../../freewrap/win32/freewrapTCLSH.exe minimosd_cfg.tcl
rm minimosd_cfg.tcl
cd ..
zip -r MinimOSDx2-$ver.zip MinimOSDx2-$ver
