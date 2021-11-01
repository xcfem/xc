Installing xc dependencies in Unix systems
================================================

If you're using Debian strecht or Ubuntu 18 or 20 you can use scripts which automate the installation of the packages needed. You will find further instructions in <https://github.com/xcfem/xc/blob/master/install/install.scripts.linux.md>

If you use a different Unix/Linux distribution you will install all packages needed for your distribution. You can still use the above scripts as a guide on what you'll need to install.

xc is a C++ (and fortran) library, so a C++ and a Fortran compiler are necessary. You can use the library as a Python module so you need a Python interpreter too. The library was tested with GNU C++ and Fortran version 4.8.2


* You will also need CMake (<http://www.cmake.org/>).
* The library is supposed to be used over Linux, maybe it's possible to use it with other Unix flavours.
* Libraries needed (normally you'll find OS packages to install them):

	- Boost c++ libraries (<http://www.boost.org/>).
	- CImg C++ Template Image Processing Toolkit (cimg-dev).
	- CGAL computational geometry algorithms library <http://www.cgal.org/> (libcgal-dev).
    - Git version control system (<https://git-scm.com/>).
	- GLib library (libglib2.0-dev).
	- GMSH Three-dimensional finite element mesh generator (<https://gmsh.info>).
	- GTK GIMP Tookit (libgtk2.0-dev, libgtkgl2.0-dev, libgtkglextmm-x11-1.2-dev, libgtkmm-2.4-dev).
	- GNU plotutils libraries (libplot gnuplot).
	- GNU Triangulated Surface Library (libgts-bin libgts-dev).
	- Multiple precision floating-point computation developers tools (libmpfr-dev).
	- Multiprecision arithmetic library developers tools (libgmp3-dev).
	- MySQL C++ library bindings (libmysql++-dev).
	- SQLite 3 development files (libsqlite3).
	- VTK Visualization ToolKit <http://www.vtk.org> (libvtk5-dev).
	- X11 (libX11-dev).

	 You can find a list of the Ubuntu (or Debian) packages you need to
	 fulfil the installation in "doc/needed_packages.txt". If you have
	 a Debian, Ubuntu or (probably) Linux Mint distribution you can
	 run the "doc/install_*whatever*_packages.sh" to install all
	 needed packages (*whatever* can be debian, ubuntu16,...).
