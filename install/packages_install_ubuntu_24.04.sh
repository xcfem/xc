scriptName=`basename "$0"`
# Must run as root so that we can shutdown backuppc and mount drives
if [ $(whoami) != "root" ]; then
    echo "You must run this script as root."
    echo "Use 'sudo sh $scriptName' and enter the password when prompted."
    exit 1
fi

# Extract version information.
version () { echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'; }

# print information about non-free packages
echo "Some packages are in the \"contrib\" and \"non-free\" areas of the Ubuntu/Debian distribution so these areas should be included in the sources.list file before running this script."

# verify that the user wants to continue, but skip this step if a
# parameter DoNotAsk was given with script start.
if [ "$1" != "DoNotAsk" ]; then
    read -p "Continue (y/n)?" REPLY
    if [ $REPLY != "y" ]; then
        echo "Exiting..."
        exit 1
    fi
fi

sudo apt-get update -y

# add universe repository so packages like gtk3 can be found
sudo install software-properties-common
sudo add-apt-repository universe
sudo apt-get update -y

build_essential="\
    build-essential  \
    zlib1g-dev       \
    libncurses5-dev  \
    libgdbm-dev      \
    libnss3-dev      \
    libssl-dev       \
    libreadline-dev  \
    libffi-dev       \
    wget"
sudo apt-get install -y $build_essential

# packages installed by package manager apt-get
# tested on Ubuntu 18.04 Bionic Beaver
packages_build="\
    git         \
    cmake       \
    g++         \
    gfortran    \
    libboost-all-dev"
sudo apt-get install -y $packages_build

# libdb-dev: Berkeley DB library
packages_lib="\
    libarpack2-dev              \
    libarpack++2-dev            \
    libcgal-dev                 \
    libcgal-qt5-dev             \
    libdb-dev                   \
    libf2c2-dev                 \
    libglib2.0-dev              \
    libgmp3-dev                 \
    libgtk-3-dev                \
    libgtkmm-3.0-dev            \
    libgts-bin                  \
    libgts-dev                  \
    liblapack-dev               \
    libmumps-dev                \
    libmpfr-dev                 \
    libmysql++-dev              \
    libplot-dev                 \
    libsqlite3-dev              \
    libsuperlu-dev              \
    libsuitesparse-dev          \
    libvtk9-dev                 \
    libvtk9-qt-dev              \
    libx11-dev                  \
    libspectra-dev              \
    libmetis-dev"
sudo apt-get install -y $packages_lib

packages_dev="\
    python3-dev \
    cimg-dev    \
    petsc-dev   \
    tcl-dev"
sudo apt-get install -y $packages_dev

packages_python="\
    python3-vtk9         \
    python3-numpy        \
    python3-scipy        \
    python3-sympy        \
    python3-matplotlib   \
    python3-pandas       \
    python3-sklearn      \
    python3-flatlatex    \
    python3-pip"
sudo apt-get install -y $packages_python

packages_div="\
    gnuplot \
    python3-pip \
    bc \
    graphicsmagick-imagemagick-compat"
# - bc is needed by xc_utils verification tests
# - graphicsmagick-imagemagick-compat is needed to convert between image
#   formats.
sudo apt-get install -y $packages_div

packages_python="\
    mayavi2 \
    python3-ezdxf \
    python3-cairo \
    python3-cairo-dev"
sudo apt-get install -y $packages_python

# free disk space by cleaning install files
sudo apt-get clean

# Ubuntu 24.04 refuses to install the following packagres system-wide
# unless you use --break-system-packages which seems too agressive.
# For the moment we comment out the installation
# sudo -H pip3 install pyexcel
# sudo -H pip3 install pyexcel-ods

# GMSH installation.
GMSH_REQUIRED_VERSION="4.8.4"
gmshHeader="/usr/local/include/gmsh.h"
if [ ! -f "$gmshHeader" ] # GMSH not installed.
then # Install GMSH
    # Check ubuntu package version.
    tmp=`sudo apt-cache policy gmsh | sed -n '3p' | cut -c 14-`
    GMSH_PKG_VERSION="$(echo $tmp | cut -d'+' -f1)"
    if [ $(version $GMSH_PKG_VERSION) -ge $(version $GMSH_REQUIRED_VERSION) ]; then
	echo "$0: gmshHeader '${gmshHeader}' will be installed from Ubuntu repositories."
	packages_gmsh="\
	    libgmsh-dev                 \
	    python3-gmsh                \
	    python3-pygmsh                \
	    libgmsh4.12"
	sudo apt-get install -y $packages_gmsh
    else
	echo "\e[31m\e[1mPackaged version: $GMSH_PKG_VERSION is too old. GMSH 4.8 or later required."
        echo "See: https://github.com/xcfem/xc/blob/master/install/gmsh_install.md"
        echo "to install a more recent version.\e[0m"
    fi    
else
    echo "$0: gmshHeader '${gmshHeader}' already installed. Skipping."    
fi
