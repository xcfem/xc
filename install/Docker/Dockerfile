FROM ubuntu:24.04

# Set apt to non iteractive during build time
ENV DEBIAN_FRONTEND=noniteractive

ARG MAKE_WORKERS=4
ARG RUN_TESTS=0
ARG XC_COMMIT_ID=0081f13

WORKDIR /xc_install

RUN \
    # Update and upgrade apt
    apt update -y && apt upgrade -y && \
    # Install sudo: Only need to be done on Docker
    apt install -y sudo && \
    # Add universe repository so it can find gtk3-dev packages
    sudo apt install -y software-properties-common && sudo add-apt-repository universe && sudo apt update && \
    # Install Python3 and make is default
    sudo apt install -y python3 python-is-python3 && \
    # Install essential packages
    sudo apt install -y build-essential zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev wget && \
    sudo apt install -y git cmake g++ gfortran libboost-all-dev && \
    # Install XC Dependencies
    sudo apt install -y libarpack2-dev libarpack++2-dev libcgal-dev libcgal-qt5-dev libdb-dev libf2c2-dev libglib2.0-dev libgmp3-dev libgtk-3-dev libgtkmm-3.0-dev libgts-bin libgts-dev liblapack-dev libmumps-dev libmpfr-dev libmysql++-dev libplot-dev libsqlite3-dev libsuperlu-dev libsuitesparse-dev libvtk9-dev libvtk9-qt-dev libx11-dev libspectra-dev libmetis-dev && \
    sudo apt install -y python3-dev cimg-dev petsc-dev tcl-dev && \
    sudo apt install -y python3-vtk9 python3-numpy python3-scipy python3-sympy python3-matplotlib python3-pandas python3-sklearn python3-flatlatex python3-pip && \
    sudo apt install -y gnuplot python3-pip bc graphicsmagick-imagemagick-compat && \
    sudo apt install -y mayavi2 python3-ezdxf python3-cairo python3-cairo-dev && \
    sudo apt install -y libgmsh-dev python3-gmsh python3-pygmsh libgmsh4.12 && \
    sudo apt clean && \
    # Clone Repository
    base_dir=`pwd` && \
    git clone https://github.com/xcfem/xc/ xc && \
    cd xc && \
    # Switch to the specific branch
    git switch --detach "$XC_COMMIT_ID" && \
    # RUN CMake and build using Make
    mkdir $base_dir/build && \
    cd $base_dir/build && \
    cmake $base_dir/xc/src && \
    make -j "$MAKE_WORKERS" && \
    sudo make install && \
    # Install XC
    cd $base_dir/xc/python_modules && \
    sudo sh local_install.sh && \
    # Verify installation running tests
    if [ "$RUN_TESTS" -ne 0 ]; then cd $base_dir/xc/verif && sh run_verif.sh; fi

# Set the volume and make it the working directory
VOLUME /home/ubuntu
WORKDIR /home/ubuntu

# Reset apt iteractive for the final image
ENV DEBIAN_FRONTEND=dialog