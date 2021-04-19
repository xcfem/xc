![XC](./doc/logo/xc_logo_alpha_channel_100x100.png)

Open source finite element analysis program.
============================================

## Overview
You can see some screenshots <a href="https://github.com/xcfem/xc/tree/master/doc/screenshots" target="_new">here</a>.

The objective is to develop a robust structural analysis tool, which is owned by the community, and allows the users to improve and adapt it for their own needs. Open-source software (or, even better, free software) is the natural evolution of the tools structural engineers have been using forever.


It's all about developing tools that allow us to keep going forward (much in the sense of <a href="https://youtu.be/QhGUtJFl0HM" target="_new">designing for strength, economy and beauty</a>) instead of repeating the same old solutions but in a faster way.

## Features
The main capabilities of the program:

- Geometry modeling and mesh generation tools.
- OD, 1D, 2D and 3D elements.
- Linear and non-linear analysis, static and dynamic.
- Fiber section models (modelization of RC members,...).
- Activation an deactivation of elements (construction phases,...).
- Tools for implementing structural codes (in progress):
    - Reinforced concrete structures: Eurocode 2, ACI 318, EHE, SIA 262
	- Steel structures: Eurocode 3, AISC 360, EAE, SIA 263
	- Wood structures and masonry structures (in progress...).
- Interface with Industry Foundation Classes through <a href="http://ifcquery.com/" target="_new"> IFC++</a> (in progress...).
- Interface with SCIA Engineering (in progress...).
- Component-based finite element method (CBFEM) for analysis and design of steel conections (in progress...).

![CBFEM: connection capacity factor](./doc/screenshots/connection_capacity_factor.png)

## Installation
See <a href="https://github.com/xcfem/xc/blob/master/install/install.txt" target="_new">install</a>

You can also find <a href="https://hub.docker.com/repository/docker/antonsurv/xcfem" target="_new">Docker images</a> created by Anton Surviyanto. The source code of the Dockerfile is <a href="https://github.com/antonsurv/docker-antonsurv-xcfem" target="_new">here</a>

### Dependencies
Don't worry, if you have a decent *nix system the installation is straightforward. The program is based on:
<ul>
<li> <a href="http://www.python.org/" target="_new">Python.</a> </li>
<li> <a href="http://opensees.berkeley.edu" target="_new">OpenSees.</a> </li>
<li> <a href="http://www.vtk.org" target="_new">Visualization ToolKit (VTK).</a> </li>
<li> <a href="http://www.boost.org" target="_new">Boost c++ libraries.</a> </li>
<li> <a href="http://www.cgal.org" target="_new">Computational geometry algorithms library (CGAL).</a> </li>
<li> <a href="https://gmsh.info" target="_new">Gmsh.</a> </li>
<li> <a href="http://www.numpy.org/" target="_new">NumPy.</a> </li>
<li> <a href="http://www.scipy.org/" target="_new">SciPy.</a> </li>
<li> <a href="http://www.latex-project.org/" target="_new">LaTeX.</a> </li>
<li> <a href="http://matplotlib.org/" target="_new">Matplotlib.</a> </li>
<li> <a href="http://www.mpfr.org/" target="_new">libmpfr.</a> Multiple precision floating-point computation developers tools (libmpfr).</li>
<li> <a href="https://gmplib.org/" target="_new">libgmp.</a> Multiple precision arithmetic library developers tools (libgmp).</li>
<li> <a href="https://code.launchpad.net/~bdb/berkeley-db/trunk" target="_new"> Berkeley DB.</a> Berkeley database libraries for C++ (libdb,libdb-dev).</li>
<li> SQLite 3 development files (libsqlite3).</li>
<li> <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack</a> (Fortran77 subroutines to solve large scale eigenvalue problems; libarcpack2-dev).</li>
<li> <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack++</a> (object-oriented version of the ARPACK package).</li>
<li> <a href="http://http://www.netlib.org/lapack/" target="_new"> Lapack</a> Library of linear algebra routines; (liblapack-dev).</li>
<li> <a href="https://launchpad.net/ubuntu/+source/superlu" target="_new"> SuperLU</a> (Direct solution of large, sparse systems of linear equations libsuperlu3-dev).</li>
<li> <a href="http://faculty.cse.tamu.edu/davis/suitesparse.html" target="_new"> Suitesparse</a> (collection of libraries for computations for sparse matrices).</li>
<li> <a href="http://www.mcs.anl.gov/petsc/petsc-as" target="_new"> PETSC</a> (Portable, Extensible Toolkit for Scientific Computation).</li>
<li> <a href="http://crd-legacy.lbl.gov/~osni/#Software" target="_new"> Skypack</a> (SKYline PACKage).</li>
<li> <a href="http://rene.ma.utexas.edu/CNA/ITPACK/" target="_new"> ITPACK</a> (collection of subroutines for solving large sparse linear systems by adaptive accelerated iterative algorithms).</li>
<li> <a href="https://github.com/xcfem/xc_utils" target="_new"> xc_utils</a> Basic routines and utilities for XC.</li>
</ul>


## Where to start
The programs lacks of a user manual (would you help us with that?). Anyway you can start with some simple examples you'll find <a href= "https://github.com/xcfem/xc_examples" target="_new"> here</a>.

## Links
You can take a look to the project <a href="https://sites.google.com/site/xcfemanalysis/" target="_new">home</a>, to the <a href="https://xcfem.github.io/XCmanual/" target="_new">Python documentation</a> or the <a href="https://codedocs.xyz/xcfem/xc/index.html" target="_new">C++ API documentation</a> or to our <a href="http://www.xcengineering.xyz/html_files/software.html" target="_new"> XC page in the company site.</a> (all of them are work in progress ;-). 





<!--  LocalWords:  XC
 -->
