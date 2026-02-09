# C++ source code

there is a bit of fluff in what i have given you and it needs to be
cleaned out. NEXT TIME.

## Contents
As to the organisation: for every class there will eventually be 2 files, a class.h, and a class.cc. all files are in the following subdirectories:

### Documentation
doc: contains some files concerning documentation, model.tex a latex
file and model.ps the postscript (every class does not yet have an upto
date class file .. prof. fenves is deciding on the format the
documentation will have .. the .ps file is around 200 pages if you want
to look at it and give your own comments)

### Domain
contains the classes for the domain.

### Material
Classes that represent the materials abstraction.

### Preprocessor
Classes used to build the finite element model.

### Reliability
Code that deal with realiability analysis.

### Solution
contains the code that deals with the solution of the finite element problem.

### Utility
Utility classes and functions.

## References

- [Finite Element Method – What Is It?](https://www.simscale.com/blog/2016/10/what-is-finite-element-method)
- [Finite Element Method (Wikipedia)](https://en.wikipedia.org/wiki/Finite_element_method)
- [Introduction to Finite Element Analysis](https://fem-heroes.org/introduction-to-fea/)
- [Pre-processing](https://fem-heroes.org/preprocessing)
- [Solver](https://fem-heroes.org/solver)
- [Post-processing](https://fem-heroes.org/postprocessing/)
- [How to Find a Memory Leak in OpenSees](https://portwooddigital.com/2022/05/29/how-to-find-a-memory-leak-in-opensees/)
- [Indentation style](https://en.wikipedia.org/wiki/Indentation_style)
- [A Better Way to Find Memory Leaks in OpenSees](https://portwooddigital.com/2023/12/27/a-better-way-to-find-memory-leaks-in-opensees/)

### PFEM (not yet implemented in XC)
- [The PFEM in OpenSees](https://portwooddigital.com/2023/11/06/the-pfem-in-opensees/)
- [Just Fillin’ Up the Tank](https://portwooddigital.com/2024/08/08/just-fillin-up-the-tank/)
- [Sloshing Around](https://portwooddigital.com/2024/11/03/sloshing-around/)
- [Hydrostatic Loading on a Wall](https://portwooddigital.com/2024/11/13/hydrostatic-loading-on-a-wall/)

### Remarks on code:
- XXX: Continue working here.
- TODO: pending tast.
- TOCHECK: something we need to check to be sure that the code works as intended- WARNING: something that is not complete yet and will need to be revisited in a future. 

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



