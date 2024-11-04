# Verification tests for beam-column elements

## Geometric nonlinearity for beam column elements.
To capture the P-little delta effecs in beam elements you need to use at least six ordinary beam column elements (ForceBeamColumn, ElasticBeam, etc.) because they are not capable of handling geometric nonlinearity _within_ the basic system. As an alternative, we need to implement [ForceBeamColumnCDBI](https://github.com/OpenSees/OpenSees/blob/master/SRC/element/forceBeamColumn/ForceBeamColumnCBDI3d.h) and [MixedBeamColumn](https://github.com/OpenSees/OpenSees/tree/master/SRC/element/mixedBeamColumn) which can handle geometric nonlinearity inside the element itself.

## References

- [Euler–Bernoulli beam theory](https://en.wikipedia.org/wiki/Euler%E2%80%93Bernoulli_beam_theory)
- [General beam theory](https://web.mit.edu/16.20/homepage/8_GeneralBeamTheory/GeneralBeamTheory_files/module_8_with_solutions.pdf.moved)
- [A Vector in the x-z Plane](https://portwooddigital.com/2020/08/08/a-vector-in-the-x-z-plane/)
- [Plastic Rotation](https://portwooddigital.com/2021/03/14/plastic-rotation/)
- [A tale of two element formulations](https://portwooddigital.com/2020/02/23/a-tale-of-two-element-formulations/)
- [Meshing for Column Loads](https://portwooddigital.com/2020/05/10/meshing-for-column-loads/)
- [Direct Moment-Curvature](https://portwooddigital.com/2021/09/26/direct-moment-curvature/)
- [Slender Things](https://portwooddigital.com/2021/11/04/slender-things/)
- [A Complicated Equivalent](https://portwooddigital.com/2022/05/01/a-complicated-equivalent/)
- [A Solution, Just Not The Solution](https://portwooddigital.com/2022/05/22/a-solution-just-not-the-solution)
- [Elastic Shear Beams in OpenSees](https://portwooddigital.com/2022/07/03/elastic-shear-beams-in-opensees/)
- [Shear Verse, Same as the First](https://portwooddigital.com/2022/07/10/shear-verse-same-as-the-first/)
- [Force-Based Element Moment Release](https://portwooddigital.com/2022/10/16/force-based-element-moment-release/)
- [Simple Loads on a Cantilever](https://portwooddigital.com/2022/11/04/simple-loads-on-a-cantilever/)
### Material nonlinearity
- [A Tale of Two Element Formulations](https://portwooddigital.com/2020/02/23/a-tale-of-two-element-formulations/)
- [Force-based Element vs. Displacement-based Element](https://opensees.berkeley.edu/wiki/images/c/c5/FBEvsDBE.pdf)
### Geometric nonlinearity
- [Geometric Transformation](https://portwooddigital.com/2022/11/15/geometric-transformation/)
- [One and Only One](https://portwooddigital.com/2023/11/18/one-and-only-one/)
### Buckling
- [Meshing for Column Loads](https://portwooddigital.com/2020/05/10/meshing-for-column-loads/)
### Lateral-torsional buckling
- [Torsional Buckling](https://portwooddigital.com/2024/11/01/torsional-buckling/)
