#!/bin/bash

##########################################################
# script for compiling gmsh on linux operating system
# it should run on any linux which has all needed packages installed

# Install dependencies
sudo apt-get install libfltk1.3-dev


##########################################################
# get current directory and create a new for building xc
base_dir=`pwd`
echo "Base dir: ${base_dir}"
rm -rf build_gmsh
mkdir build_gmsh

##########################################################
# get the number of physical CPUs
physicalCpuCount=$(getconf _NPROCESSORS_ONLN)
cpusToUse=$((physicalCpuCount-1))
cpusToUse=$((cpusToUse>1 ? cpusToUse : 1))
echo "Will use ${cpusToUse} CPUs"

##########################################################
# clone gmsh
cd $base_dir
git clone https://gitlab.onelab.info/gmsh/gmsh.git

##########################################################
# build gmsh
cd $base_dir
cd build_gmsh
cmake -DENABLE_BUILD_DYNAMIC=1 -DENABLE_FLTK=1 ../gmsh
make -j ${cpusToUse}
sudo make install
