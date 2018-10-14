XC installation on Linux operating system
=========================================

## Precompiled packages
ATM there are no precompiled packages for no Linux distribution available. 
Means you need to compile XC yourself. This is quite straight forward with the
documentation provided.


## Patience
Please have patience. It takes some time and you may encounter some problems. 
If you have any problem at installation process, feel free to add an issue at https://github.com/xcfem/xc/issues


## Introduction
### Installations steps
- install all needed depencies to be able to compile XC
- get the source code
- compile the source code
- install the programm
- verify the installation by running some verification tests


### Scripts
Compiling XC is straight forward. We provide scripts which automate the tasks above. 
If you are familar with git, cmake and make, you can skip any further description 
in this file and dirctly head over to the scripts. They should do the job for you without any hasle. 
You will find further instructions in https://github.com/xcfem/xc/install/install.scripts.linux.md


## Detailed description
### Install dependencies
You will need to install all packages needed for your distribution. The package installation scripts does
install them for various Linux distributions. Your can run the script or copy the contents in a Linux shell.
Just do what ever fits best for you.

Some dependencies needed are:

- C++ and a Fortran compiler as well as cmake and a Pythoninterpreter
- VTK Visualization ToolKit (<http://www.vtk.org>)
- Boost c++ libraries (<http://www.boost.org>)
- f2c Fortran to C translator
- CGAL (computational geometry algorithms library <http://www.cgal.org/>)
- GNU plotutils libraries
- Multiple precision floating-point computation developers tools 
- Multiprecision arithmetic library developers tools
- Arpack Fortran77 subroutines to solve large scale eigenvalue problems
- Arpack++ C++ interface for the Arpack library
- Lapack (Library of linear algebra routines)
- SuperLU (Direct solution of large, sparse systems of linear equations)
- Suitesparse (collection of libraries for computations for sparse matrices)


## More dependencies ... xc_base and xc_utils
XC has another two depencies which will not be available as packages. These are xc_base and xc_utils.
They need to be compiled and installed before XC. We will gone do this in conjunction with XC


## Get the source code
First make a directroy in your home directroy and go inside. Following the commands for all needed steps.
foo@bar is common used term. You normally see your username and the name of the computer you sit at.
After this you will see the directory your are in. Check this before you copy a command. If you would 
like to copy more than one line at once you may open the XC install script xc_code_install.sh and copy the
commands from there.
```console
foo@bar:/somewhere$
foo@bar:/somewhere$  cd ~
foo@bar:~$
foo@bar:~$  mkdir build_xc
foo@bar:~$  cd build_xc
foo@bar:~/build_xc$
```

It is time to get the source code now. Git will clone the source code of xc_base, xc_utils and xc in the current directory
```console
foo@bar:~/build_xc$  git clone https://github.com/xcfem/xc_basic/ xc_basic
foo@bar:~/build_xc$  git clone https://github.com/xcfem/xc_utils/ xc_utils
foo@bar:~/build_xc$  git clone https://github.com/xcfem/xc/ xc
```


## Build and install xc_base
As said before we need to build and install xc_base and xc_utils before. This is very straight forward.
See the commands needed for this below
### xc_base
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  mkdir build-xc_basic
foo@bar:~/build_xc$  cd build-xc_basic
foo@bar:~/build_xc/build-xc_basic$
foo@bar:~/build_xc/build-xc_basic$  cmake ../xc_basic/src
foo@bar:~/build_xc/build-xc_basic$  make -j 4  # depending on the core count of your machine put in more
foo@bar:~/build_xc/build-xc_basic$  sudo make install
foo@bar:~/build_xc/build-xc_basic$  cd ..
foo@bar:~/build_xc$
```

### xc_utils
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  mkdir build-xc_utils
foo@bar:~/build_xc$  cd build-xc_utils
foo@bar:~/build_xc/build-xc_utils$
foo@bar:~/build_xc/build-xc_utils$  cmake ../xc_utils/src
foo@bar:~/build_xc/build-xc_utils$  make -j 4  # depending on the core count of your machine put in more
foo@bar:~/build_xc/build-xc_utils$  sudo make install
foo@bar:~/build_xc/build-xc_utils$  cd ../..
foo@bar:~/build_xc$
```

We still need to install the Python modules from xc_utils.
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  cd xc_utils/python_modules
foo@bar:~/build_xc/xc_utils/python_modules$
foo@bar:~/build_xc/xc_utils/python_modules$  sudo sh local_install.sh
foo@bar:~/build_xc/xc_utils/python_modules$  cd ../..
foo@bar:~/build_xc$
```


## Build and install XC
Building and installing XC works the same way as xc_utils before.

```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  mkdir build-xc
foo@bar:~/build_xc$  cd build-xc
foo@bar:~/build_xc/build-xc$
foo@bar:~/build_xc/build-xc$  cmake ../xc/src
foo@bar:~/build_xc/build-xc$  make -j 4  # depending on the core count of your machine put in more
foo@bar:~/build_xc/build-xc$  sudo make install
foo@bar:~/build_xc/build-xc$  cd ../..
foo@bar:~/build_xc$
```

XC needs to install some Python modules too.
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  cd xc/python_modules
foo@bar:~/build_xc/xc/python_modules$
foo@bar:~/build_xc/xc/python_modules$  sudo sh local_install.sh
foo@bar:~/build_xc/xc/python_modules$  cd ../..
foo@bar:~/build_xc$
```

If you had no error message XC should be installed on your machine now. It is time ot test if 
everything works as aspected.


## Run verification tests
### xc_utils
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  cd xc_utils/verif
foo@bar:~/build_xc/xc_utils/verif$
foo@bar:~/build_xc/xc_utils/verif$  sh run_verif.sh
foo@bar:~/build_xc/xc_utils/verif$  cd ../..
foo@bar:~/build_xc$
```

### xc
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  cd xc/verif
foo@bar:~/build_xc/xc/verif$
foo@bar:~/build_xc/xc/verif$  sh run_verif.sh
foo@bar:~/build_xc/xc/verif$  cd ../..
foo@bar:~/build_xc$
```



