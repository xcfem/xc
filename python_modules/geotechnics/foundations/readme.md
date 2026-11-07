# Foundation design

Code for foundation design.

## Undrained shear strength ($s_u$) vs cohesion ($c$)

Undrained shear strength ($s_u$) is the maximum shear stress a saturated clay can withstand under quick loading conditions where pore water cannot escape, whereas cohesion (or $c$) is a fundamental strength parameter in the Mohr-Coulomb failure criterion representing a soil's inherent ability to stick together. In short-term undrained total stress analysis, clay is treated as purely cohesive with a friction angle of zero ($\phi= 0$), making the undrained shear strength numerically equal to the undrained cohesion ($s_u= c_u$).

## PySimple1 material. Recommended values for Cd.

- Soft Clays (SoilType = 1): Stable gaps commonly form behind the pile during cyclic loading. Based on back-analyses of experimental data (such as the centrifuge tests conducted by Wilson in 1998), it is standard practice to adopt a value of $C_d= 0.3$. This implies that the residual drag resistance inside the gap is limited to 30% of the ultimate soil capacity.
- Sands (SoilType = 2): Drained sands tend to cave in and backfill the voids around the pile as it displaces. Because of this self-healing behavior, an open-gap condition rarely persists. Therefore, a value of ⁠$C_d\geq 1.0$ is typically used to reflect that the material quickly recovers its full resistance upon reversing direction or closing the gap.

## References

- [Soil consolidation](https://en.wikipedia.org/wiki/Soil_consolidation)
- [The pressuremeter in geotechnical practice](https://onlinepubs.trb.org/Onlinepubs/trr/1986/1089/1089-002.pdf)
- [Standard penentration test (SPT) for foundation design](https://structville.com/standard-penetration-test-spt)
- [Unconfined Compressive Strength Test](https://www.engineeringcivil.com/unconfined-compressive-strength-test.html)
- [Soil consistency](https://www.fao.org/fishery/docs/CDrom/FAO_Training/FAO_Training/General/x6706e/x6706e08.htm)
- [Guía de cimentaciones en obras de carretera](https://www.mitma.es/recursos_mfom/0710401.pdf)
- [Sand](https://en.wikipedia.org/wiki/Sand)
- [Silt](https://en.wikipedia.org/wiki/Silt)
- [Clay](https://en.wikipedia.org/wiki/Clay)
- [Peat](https://en.wikipedia.org/wiki/Peat)
- [Design life](https://www.designingbuildings.co.uk/wiki/Design_life)

### Piles
- [Guía para el proyecto y la ejecución de micropilotes en obras de carretera](https://www.mitma.es/recursos_mfom/0710200.pdf)
- [Deep foundation](https://en.wikipedia.org/wiki/Deep_foundation)
- [Modelling piles in FEM-Design](https://odr.chalmers.se/items/1e205dff-93c5-428e-b1aa-d2a3602ab181)

#### Piles under lateral loads

- [Recommended Practice for Planning, Designing and Constructing Fixed Offshore Platforms-Working Stress Design](https://www.academia.edu/105904042/Recommended_Practice_for_Planning_Designing_and_Constructing_Fixed_Offshore_Platforms_Working_Stress_Design)
- [Bulletin no. 12: The Ultimate Resistance of Rigid Piles against Transversal Forces & Model Tests with Transversally Loaded Ridig Piles in Sand](https://www.geo.dk/bibliotek/196x/the-ultimate-resistance-of-rigid-piles-against-transversal-forces-model-tests-with-transversally-loaded-ridig-piles-in-sand/#t=article)
- [Pile foundations in liquefied and laterally spreading ground during earthquakes: centrifuge experiments & analyses](https://faculty.engineering.ucdavis.edu/boulanger/wp-content/uploads/sites/71/2014/09/Boulanger_et_al_CGM03_01.pdf)
- [Single Piles and Pile Groups Under Lateral Loading](https://books.google.es/books?id=buTqBgAAQBAJ&printsec=frontcover&redir_esc=y#v=onepage&q&f=false)

#### Piles under vertical loads

- [Bearing capacity and settlement of pile foundations.Meyerhof G.G. (1976).](https://ascelibrary.com/doi/10.1061/AJGEB6.0000243)
- [Manual on Estimating Soil Properties for Foundation Design. Electrical Power Research Institute. EPRI EL-6800, Project 1493-6 Final Report.](https://www.geoengineer.org/storage/publication/20489/publication_file/2745/EL-6800.pdf)
- [Load-transfer criteria for numerical analysis of axially loaded piles in sand](https://www.researchgate.net/publication/34879241_Load-transfer_criteria_for_numerical_analysis_of_axially_loaded_piles_in_sand)
