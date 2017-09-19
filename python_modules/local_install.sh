#!/bin/sh
mkdir -p ./extensions
ln_xc=./extensions/xc.so
if [ ! -h $ln_xc ]; then
  echo "Making $ln_xc symlink."
  ln -si ../../lib/xc.so $ln_xc
fi
sudo python setup.py install --prefix=/usr/local --record installed_files.txt
echo "Updating installed files history."
cat installed_files_history.txt installed_files.txt | sort | uniq > tmp.txt
mv tmp.txt installed_files_history.txt
sudo rm installed_files.txt
