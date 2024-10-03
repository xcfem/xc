# Gmsh and FreeCAD-daily
If you intend to a recent version of FreeCAD to create the geometry of your finite element models, the packaged version of Gmsh poses a problem. The Ubuntu 24.04 [Gmsh](https://gmsh.info/) package uses an version of OpenCascade libraries older that the one [FreeCAD-daily](https://launchpad.net/~freecad-maintainers/+archive/ubuntu/freecad-daily) uses. That makes those packages incompatible.

## The solution
To get rid of this problems you can install Gmsh using one of the following methods:

- Use the `gmsh_code_install.sh` script provided in the `install` folder.
- Use one of the methods proposed [here](https://gmsh.info/#Download), but make sure the C/C++ headers (`usr/local/include/gmsh.h`, `usr/local/include/gmshc.h`, `usr/local/include/gmsh.h_cwrap`) are installed in your system.
