# Load combinations
A load combination results when more than one load type acts on the structure. Building codes usually specify a variety of load combinations together with load factors (weightings) for each load type in order to ensure the safety of the structure under different maximum expected loading scenarios.

The modules in this folder define the combination rules for different building codes.

## Eurocode 0

### Ultimate limit states
Excerpt from clause 6.4.1. The following ultimate limit states shall be verified as relevant:

 - EQU: Loss of static equilibrium of the structure or any part of it considered as a rigid body.
 - STR: Internal failure or excessive deformation of the structure or structural members, including footings, piles, basement walls, etc., where the strength of construction materials of the structure governs.
 - GEO: Failure or excessive deformation of the ground where the strengths of soil or rock are significant in providing resistance.
 - FAT: Fatigue failure of the structure or structural members.
 - UPL: loss of equilibrium of the structure or the ground due to uplift by water pressure (buoyancy) or other vertical actions.
 - HYD: hydraulic heave, internal erosion and piping in the ground caused by hydraulic gradients.
### Design values of actions (on bridges).
 * Set A: Static equilibrium (EQU) on bridges should be verified using the design values of actions in Table A2.4(A).
 * Set B: Design of bridge structural members (STR) not involving geotechnical actions should be verified using the design values of actions in Table A2.4(B).
 * Set C: Design of structural members (footings, piles, piers, side walls, wing walls, flank walls and front walls of abutments, ballast retention walls, etc.) (STR) involving geotechnical actions and the resistance of the ground (GEO) should be verified using one only of the following three approaches supplemented, for geotechnical actions and resistances, by EN 1997:
     - Approach 1: Applying in separate calculations design values from Table A2.4(C) and Table A2.4(B) to the geotechnical actions as well as the actions on/from the structure.
	 - Approach 2: Applying design values of actions from Table A2.4(B) to the geotechnical actions as well as the actions on/from the structure;
	 - Approach 3: Applying design values of actions from Table A2.4(C) to the geotechnical actions and, simultaneously, applying design values of actions from Table A2.4(B) to the actions on/from the structure.
 

## References
- [LoadCombination (Wikipedia)](https://en.wikipedia.org/wiki/Structural_load#Load_combinations)
- [Eurocode 0. Basis of structural desing (EU)](https://eurocodes.jrc.ec.europa.eu/showpage.php?id=130)
- [SIA 260 Bases pour l’élaboration des projets de structures porteuses (CH)](https://www.sia.ch/fr/services/articles-contributions/detail/article/norme-revisee-260-bases/)
- [Código técnico de la edificación (ES).](https://www.codigotecnico.org/pdf/Documentos/SE/DBSE.pdf)
- [Código Estructural (ES)](https://boe.es/boe/dias/2021/08/10/pdfs/BOE-A-2021-13681.pdf)
- [ASCE-7 (US)](https://www.asce.org/publications-and-news/asce-7)
- [International Building Code (US)](https://codes.iccsafe.org/content/IBC2021P1)

<!--  LocalWords:  Eurocode
 -->
