## Linear buckling analysis
Verification tests related to the computation of linear buckling modes.

### Linear buckling for beam column elements.
To capture the P-little delta effecs in beam elements you need to use at least six ordinary beam column elements (ForceBeamColumn, ElasticBeam, etc.) because they are not capable of handling geometric nonlinearity _within_ the basic system. As an alternative, we need to implement [ForceBeamColumnCDBI](https://github.com/OpenSees/OpenSees/blob/master/SRC/element/forceBeamColumn/ForceBeamColumnCBDI3d.h) and [MixedBeamColumn](https://github.com/OpenSees/OpenSees/tree/master/SRC/element/mixedBeamColumn) which can handle geometric nonlinearity inside the element itself.

### References
- [Buckling](https://en.wikipedia.org/wiki/Buckling)
- [Linear buckling in plain language!](https://enterfea.com/linear-buckling-explained/)
- [Linear buckling analysis](https://portwooddigital.com/2021/05/29/right-under-your-nose)
- [One and Only One](https://portwooddigital.com/2023/11/18/one-and-only-one/)
- [Geometric Transformation](https://portwooddigital.com/2022/11/15/geometric-transformation/)
- [Corotational Rigid Offsets](https://portwooddigital.com/2024/11/11/corotational-rigid-offsets/)
- [Critical Buckling Loads via CBDI](https://portwooddigital.com/2024/11/16/critical-buckling-loads-via-cbdi/)
 - [Minimal Plate Buckling Example](https://portwooddigital.com/2025/05/24/minimal-plate-buckling-example/)
 - [Buckling of Restrained Plates](https://portwooddigital.com/2025/05/31/buckling-of-restrained-plates/)
