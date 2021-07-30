# C++ source code

there is a bit of fluff in what i have given you and it needs to be
cleaned out. NEXT TIME.

## Contents
As to the organisation: for every class there will eventually be 2 files, a class.h, and a class.cc. all files are in the following subdirectories:

### Documentation
doc: - contains some files concerning documentation, model.tex a latex
file and model.ps the postscript (every class does not yet have an upto
date class file .. prof. fenves is deciding on the format the
documentation will have .. the .ps file is around 200 pages if you want
to look at it and give your own comments) 

### Domain
contains the classes for the domain. all in subdirectores:

### Material
Classes that represent the materials abstraction.

### Solution
contains the code that deals with the solution of the finite element problem.

### Utility
Utility classes and functions.

<!-- NOTES FOR MYSELF: -->
<!-- notes for alpha cluster: -->
<!--   1) change 'ssh' in ~/remote/remote.c to 'rsh' -->
<!--   2) remove the threaded profile solver -->

<!-- notes for holden: -->
<!--   1) create a HoldenMachineBroker which starts processes on the -->
<!--      alpha cluster. -->

<!-- notes for millenium machines: -->
<!--   1) not yet working for CC compiler, need to bcopy() struct addr to -->
<!--      addr_in -->
<!--   2) the parallel domain decomposition is using ssh to start the -->
<!--      remote processes, has to be set up so don't need to supply password - -->
<!--      this can be done from mill.cs - but not from any other millenium machine! -->
<!--      they must have some problem with ssh. talk to eric again. -->
<!--      rsh would be nice! see if eric will allow rsh until fix ssh? -->



