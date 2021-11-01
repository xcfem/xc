# Gmsh installation and XC

## Gmsh version packaged with Ubuntu
The [Gmsh](https://gmsh.info/) package shipped with Ubuntu 20.04 corresponds to version 4.4.1. XC started to use Gmsh meshing capabilities in September 2020, and now it makes an intensive use of those capabilities. As a consequence, it requires Gmsh version 4.8 or later.



## Gmsh and FreeCAD-daily
If you intend to a recent version of FreeCAD to create the geometry of your finite element models, the packaged version of Gmsh poses another problem. The Ubuntu 20.04 [Gmsh](https://gmsh.info/) package uses an old version of OpenCascade libraries (libooct 7.3) and [FreeCAD-daily](https://launchpad.net/~freecad-maintainers/+archive/ubuntu/freecad-daily) uses a more recent version (7.5). That makes those packages incompatible.

## The solution
To get rid of this problems you can install Gmsh using one of the following methods:

- Use the `gmsh_code_install.sh` script provided in the `install` folder.
- Use one of the methods proposed [here](https://gmsh.info/#Download), but make sure the C/C++ headers (`usr/local/include/gmsh.h`, `usr/local/include/gmshc.h`, `usr/local/include/gmsh.h_cwrap`) are installed in your system.

