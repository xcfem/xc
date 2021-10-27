#!/bin/bash


##########################################################
# script for compiling xc on linux operating system
# it should run on any linux which has all needed packages installed


##########################################################
# get current directory and create a new for building xc
rm -rf build_xc
mkdir build_xc
cd build_xc
base_dir=`pwd`


##########################################################
# clone XC
cd $base_dir
git clone https://github.com/xcfem/xc/ xc


##########################################################
# build xc_basic
### LCPT MERGED INTO xc_utils 03.11.2018 so not needed anymore.
# cd $base_dir
# mkdir build-xc_basic
# cd build-xc_basic
# cmake ../xc_basic/src
# make -j 2
# sudo make install
### LCPT
## source needed as include dir for xc_utils, includes are not installed into /usr/local/include
## in xc_utils sqlite will only be found if build of xc_basic will not be removed
## or add find_packages sqlite to xc_utils, see a commit on a branch in my repo of xc_utils


##########################################################
# build xc_utils
### LCPT MERGED INTO xc 05.2021 so not needed anymore.
# cd $base_dir
# mkdir build-xc_utils
# cd build-xc_utils
# cmake ../xc_utils/src
# make -j 2
# sudo make install
# cd $base_dir/xc_utils/python_modules
# sudo sh local_install.sh


##########################################################
# build  XC
cd $base_dir
mkdir build-xc
cd build-xc
cmake ../xc/src
make -j 2
sudo make install
cd $base_dir/xc/python_modules
sudo sh local_install.sh


##########################################################
# run unit tests
cd $base_dir/xc/verif
sh run_verif.sh
