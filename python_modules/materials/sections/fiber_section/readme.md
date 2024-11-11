# Fiber section material utilities
Code that makes easier to define fiber section materials.

A fiber section object is composed of Fibers, with each fiber containing a UniaxialMaterial, an area and a location: (y) for 2D sections or (y,z) for 3D sections.
## References

- [Fiber Section (OpenSees)](https://opensees.berkeley.edu/wiki/index.php/Fiber_Section)
- [Fiber Section Centroids](https://portwooddigital.com/2021/01/24/fiber-section-centroids/)
- [Torsion with Fiber Sections](https://portwooddigital.com/2019/10/06/torsion-with-fiber-sections/)
- [How to Record Fiber Response](https://portwooddigital.com/2021/07/25/how-to-record-fiber-response)
- [Two Fibers, Five Ways](https://portwooddigital.com/2022/01/16/two-fibers-five-ways)
- [Two Fibers Explain So Much](https://portwooddigital.com/2022/02/06/two-fibers-explain-so-much/)
- [Something, Something, Something Fiber](https://portwooddigital.com/2022/02/27/something-something-something-fiber/)
- [How to Find the Neutral Axis](http://portwooddigital.com/2022/11/09/how-to-find-the-neutral-axis/)
- [Section modulus](https://en.wikipedia.org/wiki/Section_modulus)

### Shell materials
- [Plate Rebar Material](https://portwooddigital.com/2024/11/08/plate-rebar-material/)

### Reinforced concrete
<!-- language: lang-none -->

                      Reinforce concrete section
    + -------------------------------------------------------+ 
    |  o    o    o    o    o    o    o    o    o    o    o   |
    |                                                        |
    |                                                        |
    |  o    o    o    o    o    o    o    o    o    o    o   |
    + -------------------------------------------------------+ 

- [EHE](https://www.mitma.gob.es/organos-colegiados/mas-organos-colegiados/comision-permanente-del-hormigon/cph/instrucciones/ehe-08-version-en-ingles)


### Interaction diagrams
- [Implementation of interaction diagrams in the finite element open source software XC](http://www.xcengineering.xyz/blog_doc/2016_interact_diag/interactionDiagram.pdf)
- [P-M Interaction by the Book](https://portwooddigital.com/2022/06/12/p-m-interaction-by-the-book)
