#!/bin/sh
mkdir -p ./extensions
ln_xc=./extensions/xc.so
if [ ! -h $ln_xc ]; then
  echo "Making $ln_xc symlink."
  ln -si ../../lib/libxc.so $ln_xc
fi
sudo python setup.py install --prefix=/usr/local
