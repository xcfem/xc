# GMSH installation and FreeCAD-daily

## The problem
The problem is that the ubuntu 20.04 [Gmsh](https://gmsh.info/) package uses an old version of OpenCascade libraries (libooct 7.3) and [FreeCAD-daily](https://launchpad.net/~freecad-maintainers/+archive/ubuntu/freecad-daily) uses a more recent version (7.5). That makes those packages incompatible.

## The solution
To have both packages working the solution is to install FreeCAD-daily using the PPA (personal package archive) provided in the previous link and then install Gmsh using one of the following methods:

- Use the `gmsh_code_install.sh` script provided in the `install` folder.
- Use one of the methods proposed [here](https://gmsh.info/#Download), but make sure the C/C++ headers (`usr/local/include/gmsh.h`, `usr/local/include/gmshc.h`, `usr/local/include/gmsh.h_cwrap`) are installed in your system.

