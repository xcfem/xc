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
echo "Some packages are in the \"contrib\" and \"non-free\" areas of the Debian distribution so these areas should be included in the sources.list file before running this script."

# verify that the user wants to continue, but do not verify if a parameter DoNotAsk was given with script start
if [ "$1" != "DoNotAsk" ]; then
    read -p "Continue (y/n)?" REPLY
    if [ $REPLY != "y" ]; then
        echo "Exiting..."
        exit 1
    fi
fi

# packages installed by package manager apt-get
# tested on Ubuntu 18.04 Bionic Beaver
packages_build="\
    git         \
    cmake       \
    g++         \
    gfortran    \
    libboost-all-dev"
sudo apt-get install -y $packages_build

packages_lib="\
    libarpack2-dev              \
    libarpack++2-dev            \
    libcgal-dev                 \
    libcgal-qt5-dev             \
    libdb-dev                   \
    libf2c2-dev                 \
    libglib2.0-dev              \
    libgmp3-dev                 \
    libgtk2.0-dev               \
    libgtkgl2.0-dev             \
    libgtkglextmm-x11-1.2-dev   \
    libgtkglext1-dev            \
    libgtkmm-2.4-dev            \
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
    libvtk7-dev                 \
    libx11-dev                  \
    libmetis-dev"
sudo apt-get install -y $packages_lib

packages_dev="\
    python3-dev          \
    cimg-dev  \
    petsc-dev \
    tcl-dev"
sudo apt-get install -y $packages_dev

packages_python="\
    python3-vtk7         \
    python3-numpy        \
    python3-scipy        \
    python3-sympy        \
    python3-matplotlib   \
    python3-pandas       \
    python3-sklearn      \
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
	    libgmsh4"
	sudo apt-get install -y $packages_gmsh
    else
	echo "\e[31m\e[1mPackaged version: $GMSH_PKG_VERSION is too old. GMSH 4.8 or later required."
        echo "See: https://github.com/xcfem/xc/blob/master/install/gmsh_install.md"
        echo "to install a more recent version.\e[0m"
    fi    
else
    echo "$0: gmshHeader '${gmshHeader}' already installed. Skipping."    
fi


# free disk space by cleaning install files
sudo apt-get clean


# mayavi installation. Ubuntu 'mayavi' package seems to require VTK 6,
# so we use pip (to reconsider because we already use VTK 6 LCPT 24/09/2018)
sudo -H pip3 install mayavi
sudo -H pip3 install ezdxf
sudo -H pip3 install pyexcel
sudo -H pip3 install pyexcel-ods
sudo -H pip3 install dxfwrite # To replace with ezdxf
# cairo installation. 
sudo -H pip3 install pycairo

