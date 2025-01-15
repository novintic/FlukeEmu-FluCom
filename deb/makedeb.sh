#!/bin/bash

# Build debian package
# https://www.baeldung.com/linux/create-debian-package

if [ -d "$flukeEmuWx" ]; then
  rmdir -r flukeEmuWx
fi

mkdir flukeEmuWx
cd flukeEmuWx
mkdir bin
cp ../../flukeEmuWx/bin/Debug/FlukeEmuWx ./bin
cp -r ../../flukeEmuWx/bin/Debug/media ./bin

mkdir DEBIAN
cd DEBIAN
cat > control <<EOF
Package: flukeEmuWx
Version: 0.96
Section: utils
Priority: optional
Architecture: armhf
Maintainer: Novintic <novintic@github.com>
Description: Fluke 9010A Emulation for RPI
Depends: wiringpi (>= 2.52), libpulse-dev (>= 14.2), libwxgtk3.0-gtk3-0v5 (>= 3.0.5.1)
EOF

cd ../..
# build package
dpkg-deb --root-owner-group --build flukeEmuWx
