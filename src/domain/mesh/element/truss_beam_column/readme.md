# Truss and frame elements

This directory contains the classes that define the truss and frame elements available in XC.

## Contents

         - beam2d: beam elements for two-dimensional problems.
         - beam3d: beam elements for three-dimensional problems.
         - beamWithHinges: beam element with hinges.
         - dispBeamColumn: displacement formulation beam elements.
         - elasticBeamColumn: elastic beam elements.
         - forceBeamColumn: force formulation beam elements.
         - nonlinearBeamColumn: beam elements with non linear material.
         - truss: truss elements.
         - updatedLagrangianBeamColumn: beam elements with updated lagrangian formulation.

### Geometric nonlinearity for beam column elements.
To capture the P-little delta effecs in beam elements you need to use at least six ordinary beam column elements (ForceBeamColumn, ElasticBeam, etc.) because they are not capable of handling geometric nonlinearity _within_ the basic system. As an alternative, we need to implement [ForceBeamColumnCDBI](https://github.com/OpenSees/OpenSees/blob/master/SRC/element/forceBeamColumn/ForceBeamColumnCBDI3d.h) and [MixedBeamColumn](https://github.com/OpenSees/OpenSees/tree/master/SRC/element/mixedBeamColumn) which can handle geometric nonlinearity inside the element itself.

## References

- [Euler–Bernoulli beam theory](https://en.wikipedia.org/wiki/Euler%E2%80%93Bernoulli_beam_theory)
- [General beam theory](https://web.mit.edu/16.20/homepage/8_GeneralBeamTheory/GeneralBeamTheory_files/module_8_with_solutions.pdf.moved)
- [Plastic Rotation](https://portwooddigital.com/2021/03/14/plastic-rotation/)
- [A tale of two element formulations](https://portwooddigital.com/2020/02/23/a-tale-of-two-element-formulations/)
- [Meshing for Column Loads](https://portwooddigital.com/2020/05/10/meshing-for-column-loads/)
- [Direct Moment-Curvature](https://portwooddigital.com/2021/09/26/direct-moment-curvature/)
- [How to Bend Beams in 3D](https://portwooddigital.com/2023/03/14/how-to-bend-beams-in-3d/)
- [Slender Things](https://portwooddigital.com/2021/11/04/slender-things/)
- [A Complicated Equivalent](https://portwooddigital.com/2022/05/01/a-complicated-equivalent/)
- [A Solution, Just Not The Solution](https://portwooddigital.com/2022/05/22/a-solution-just-not-the-solution)
- [Elastic Shear Beams in OpenSees](https://portwooddigital.com/2022/07/03/elastic-shear-beams-in-opensees/)
- [Shear Verse, Same as the First](https://portwooddigital.com/2022/07/10/shear-verse-same-as-the-first/)
- [Simple Loads on a Cantilever](https://portwooddigital.com/2022/11/04/simple-loads-on-a-cantilever/)
- [Converging to Something](https://portwooddigital.com/2023/10/08/converging-to-something/)
### Element orientation
- [Flipping Rotation Axes](https://portwooddigital.com/2023/11/12/flipping-rotation-axes/)
- [A Vector in the x-z Plane](https://portwooddigital.com/2020/08/08/a-vector-in-the-x-z-plane/)
### Material nonlinearity
- [A Tale of Two Element Formulations](https://portwooddigital.com/2020/02/23/a-tale-of-two-element-formulations/)
- [Force-based Element vs. Displacement-based Element](https://opensees.berkeley.edu/wiki/images/c/c5/FBEvsDBE.pdf)
### Geometric nonlinearity
- [Geometric Transformation](https://portwooddigital.com/2022/11/15/geometric-transformation/)
- [One and Only One](https://portwooddigital.com/2023/11/18/one-and-only-one/)
### Buckling
- [Meshing for Column Loads](https://portwooddigital.com/2020/05/10/meshing-for-column-loads/)
- [Apples and Oranges](https://portwooddigital.com/2025/04/06/apples-and-oranges/)
 - [Linear buckling analysis](https://portwooddigital.com/2021/05/29/right-under-your-nose)
#### Lateral-torsional buckling
- [Torsional Buckling](https://portwooddigital.com/2024/11/01/torsional-buckling/)
