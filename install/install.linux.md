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
- make sure Python 3 is the default Python interpreter in your system.
- get the source code
- compile the source code
- install the programm
- verify the installation by running some verification tests


### Scripts
Compiling XC is straight forward. We provide scripts which automate the tasks above. 
If you are familar with git, cmake and make, you can skip any further description 
in this file and directly head over to the scripts. They should do the job for you without any hasle. 
You will find further instructions in https://github.com/xcfem/xc/blob/master/install/install.scripts.linux.md


## Detailed description
### Install dependencies
You will need to install all packages needed for your distribution. The package installation scripts does install them for various Linux distributions. Your can run the script or copy the contents in a Linux shell.

Just do what ever fits best for you.

Some dependencies needed are:

- C++ and a Fortran compiler as well as cmake and a Pythoninterpreter
- VTK Visualization ToolKit (<http://www.vtk.org>)
- Boost c++ libraries (<http://www.boost.org>)
- f2c Fortran to C translator
- CGAL (computational geometry algorithms library <http://www.cgal.org/>)
- Gmsh (A three-dimensional finite element mesh generator with built-in pre- and post-processing facilities <https://gmsh.info/>)
- GNU plotutils libraries
- Multiple precision floating-point computation developers tools 
- Multiprecision arithmetic library developers tools
- MUMPS: MUltifrontal Massively Parallel sparse direct Solver (<http://mumps.enseeiht.fr/>).
- Arpack Fortran77 subroutines to solve large scale eigenvalue problems
- Arpack++ C++ interface for the Arpack library
- Lapack (Library of linear algebra routines)
- SuperLU (Direct solution of large, sparse systems of linear equations)
- Suitesparse (collection of libraries for computations for sparse matrices)

### Python 3
Make sure Python 3 is the default Python interpreter for your system. If your using a Debian like distribution you can set Python 3 as the default interpreter as follows:

`sudo apt install python-is-python3`


### Get the source code
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

It is time to get the source code now. Git will clone the source code of xc in the current directory
```console
foo@bar:~/build_xc$  git clone https://github.com/xcfem/xc/ xc
```


### Build and install XC
Building and installing XC works the same way as any other program that uses cmake:

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

XC needs to install some Python modules:
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


### Run verification tests

#### xc
```console
foo@bar:~/build_xc$
foo@bar:~/build_xc$  cd xc/verif
foo@bar:~/build_xc/xc/verif$
foo@bar:~/build_xc/xc/verif$  sh run_verif.sh
foo@bar:~/build_xc/xc/verif$  cd ../..
foo@bar:~/build_xc$
```



