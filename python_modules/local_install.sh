#!/bin/sh

# Color variables
red='\033[0;31m'
green='\033[0;32m'
# Clear the color after that
clear='\033[0m'

# Check Python version.
python_version=$(python -V 2>&1 | grep -Po '(?<=Python )(.+)')
echo $python_version

python_required_version="3.0.0"

## Extract version information.
version () { echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'; }

if [ $(version $python_version) -ge $(version $python_required_version) ]; then
    echo "${green}Python version: $python_version, OK!${clear}"
else
    echo "${red}Python version: $python_version, KO!"
    echo "Python 2 is deprecated. Make sure you use Python version 3.${clear}"
    exit 1
fi

# Check compiled libraries
lib_geom_file="../lib/libgeom.so"
if test -f "$lib_geom_file"; then
    echo "${green}$lib_geom_file exists: OK!${clear}"
    lib_geom_link="../${lib_geom_file}"
else
    echo "${red}Something went wrong: $lib_geom_file doesn't exists: KO!${clear}"
    exit 1
fi
lib_xcGnuGts_file="../lib/libxcGnuGts.so"
if test -f "$lib_xcGnuGts_file"; then
    echo "${green}$lib_xcGnuGts_file exists: OK!${clear}"
else
    echo "${red}Something went wrong: $lib_xcGnuGts_file doesn't exists: KO!${clear}"
    exit 1
fi
lib_loadCombinations_file="../lib/libloadCombinations.so"
if test -f "$lib_loadCombinations_file"; then
    echo "${green}$lib_loadCombinations_file exists: OK!${clear}"
else
    echo "${red}Something went wrong: $lib_loadCombinations_file doesn't exists: KO!${clear}"
    exit 1
fi
lib_xc_base_file="../lib/libxc_base.so"
if test -f "$lib_xc_base_file"; then
    echo "${green}$lib_xc_base_file exists: OK!${clear}"
else
    echo "${red}Something went wrong: $lib_xc_base_file doesn't exists: KO!${clear}"
    exit 1
fi
lib_xc_file="../lib/xc.so"
if test -f "$lib_xc_file"; then
    echo "${green}$lib_xc_file exists: OK!${clear}"
else
    echo "${red}Something went wrong: $lib_xc_file doesn't exists: KO!${clear}"
    exit 1
fi

# Create symbolic links.
echo "Create symbolic links."
mkdir -p ./extensions
ln_geom=./extensions/geom.so
if [ ! -h $ln_geom ]; then
  echo "Making $ln_geom symlink."
  ln -si $lib_geom_link $ln_geom
fi
ln_xcGnuGts=./extensions/xcGnuGts.so
if [ ! -h $ln_xcGnuGts ]; then
  echo "Making $ln_xcGnuGts symlink."
  ln -si ../../lib/libxcGnuGts.so $ln_xcGnuGts
fi
ln_loadComb=./extensions/loadCombinations.so
if [ ! -h $ln_loadComb ]; then
  echo "Making $ln_loadComb symlink."
  ln -si ../../lib/libloadCombinations.so $ln_loadComb
fi
ln_xcBase=./extensions/xc_base.so
if [ ! -h $ln_xcBase ]; then
  echo "Making $ln_xcBase symlink."
  ln -si ../../lib/libxc_base.so $ln_xcBase
fi
ln_xc=./extensions/xc.so
if [ ! -h $ln_xc ]; then
  echo "Making $ln_xc symlink."
  ln -si ../../lib/xc.so $ln_xc
fi

# Check symbolic links
echo "Check symbolic links."
if [ -L ${ln_geom} ] && [ -e ${ln_geom} ] ; then
    echo "${green}${ln_geom} exists: OK!${clear}"
else
    echo "${red}Something went wrong: $ln_geom doesn't exists: KO!${clear}"
    exit 1
fi
if [ -L ${ln_xcGnuGts} ] && [ -e ${ln_xcGnuGts} ] ; then
    echo "${green}${ln_xcGnuGts} exists: OK!${clear}"
else
    echo "${red}Something went wrong: $ln_xcGnuGts doesn't exists: KO!${clear}"
    exit 1
fi
if [ -L ${ln_loadComb} ] && [ -e ${ln_loadComb} ] ; then
    echo "${green}${ln_loadComb} exists: OK!${clear}"
else
    echo "${red}Something went wrong: $ln_loadComb doesn't exists: KO!${clear}"
    exit 1
fi
if [ -L ${ln_xcBase} ] && [ -e ${ln_xcBase} ] ; then
    echo "${green}${ln_xcBase} exists: OK!${clear}"
else
    echo "${red}Something went wrong: $ln_xcBase doesn't exists: KO!${clear}"
    exit 1
fi
if [ -L ${ln_xc} ] && [ -e ${ln_xc} ] ; then
    echo "${green}${ln_xc} exists: OK!${clear}"
else
    echo "${red}Something went wrong: $ln_xc doesn't exists: KO!${clear}"
    exit 1
fi

# Install Python extensions.
echo "Installing Python extensions."
sudo python setup.py install --prefix=/usr/local --record installed_files.txt
echo "Updating installed files history."
if test -f installed_files_history.txt; then
    echo "${green}installed_files_history.txt already exists: OK!${clear}"
else
    echo "${green}Create installed_files_history.txt${clear}"
    touch installed_files_history.txt
    exit 1
fi
cat installed_files_history.txt installed_files.txt | sort | uniq > tmp.txt
mv tmp.txt installed_files_history.txt
sudo rm installed_files.txt
echo "${green}installed_files_history.txt updated.${clear}"
