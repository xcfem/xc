# Elements

This directory contains the classes that define the finite elements available in XC. 

## Contents

		- plane: two-dimensional elements (quads, shells, ...).
		- special: special elements (interface with FEAP elements, friction, joints, ...).
		- truss_beam_column: one-dimensional elements (truss, bars, ...).
		- utils: code useful for element programming (coordinate transformations, gauss models, ...)
		- volumentric: three-dimensional elements (hexahedrons, tetrahedrons, ...)
		- zeroLenght: zero-dimensional elements (contact elements, ...)
		
## References

- [Finite element method](https://en.wikipedia.org/wiki/Finite_element_method)
- [Inerters everywhere](https://portwooddigital.com/2021/11/13/inerters-everywhere/)
- [Stop Hogging All the RAM](https://portwooddigital.com/2022/11/16/stop-hogging-all-the-ram/)
- [Efficient, three-dimensional, four-node, macroscopic element for RC walls with shear-flexural interaction](https://kkolozvari.github.io/E-SFI-MVLEM-3D/)

### Inerters and tuned mass dampers.
- [Tuned Damper Models](https://portwooddigital.com/2023/11/11/tuned-damper-models/)

### PFEM
PFEM has not been migrated from OpenSees yet.
- [The PFEM in OpenSees](https://portwooddigital.com/2023/11/06/the-pfem-in-opensees/)
- [Just Fillin’ Up the Tank](https://portwooddigital.com/2024/08/08/just-fillin-up-the-tank/)
- [Sloshing Around](https://portwooddigital.com/2024/11/03/sloshing-around/)
- [Hydrostatic Loading on a Wall](https://portwooddigital.com/2024/11/13/hydrostatic-loading-on-a-wall/)
