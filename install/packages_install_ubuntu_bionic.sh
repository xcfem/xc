# Must run as root so that we can shutdown backuppc and mount drives
if [ $(whoami) != "root" ]; then
    echo "You must run this script as root."
    echo "Use 'sudo sh install_deb_packages.sh' and enter the password when prompted."
    exit 1
fi


# verify that the user wants to continue, but do not verify if a parameter DoNotAsk was given with script start
if [ $1 != "DoNotAsk" ]; then
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
    libdb-dev                   \
    libf2c2-dev                 \
    libglib2.0-dev              \
    libgmp3-dev                 \
    libgtk2.0-dev               \
    libgtkgl2.0-dev             \
    libgtkglextmm-x11-1.2-dev   \
    libgtkmm-2.4-dev            \
    libgts-bin                  \
    libgts-dev                  \
    liblapack-dev               \
    libmpfr-dev                 \
    libmysql++-dev              \
    libplot-dev                 \
    libsqlite3-dev              \
    libsuperlu-dev              \
    libsuitesparse-dev          \
    libvtk6-dev                 \
    libx11-dev                  \
    libmetis-dev"
sudo apt-get install -y $packages_lib

packages_dev="\
    cimg-dev  \
    petsc-dev \
    tcl-dev"
sudo apt-get install -y $packages_dev

packages_python="\
    python-vtk6         \
    python-scipy        \
    python-sympy        \
    python-matplotlib   \
    python-pandas       \
    python-sklearn      \
    python-pip"
sudo apt-get install -y $packages_python

packages_div="\
    gnuplot \
    bc"
# bc is needed by xc_utils verification tests
sudo apt-get install -y $packages_div

# install nonfree packages (ubuntu does not know nonfree, means we gone just install)
packages_nonfree="\
    libparmetis-dev"
sudo apt-get install -y $packages_nonfree


# free disk space by cleaning install files
apt-get clean


# mayavi installation. Ubuntu 'mayavi' package seems to require VTK 6,
# so we use pip (to reconsider because we already use VTK 6 LCPT 24/09/2018)
sudo -H pip install mayavi
