# Shallow foundation constraints verification tests
The tests in this folder simulate the constraints imposed by shallow foundations like footings or foundation slabs assumming a linear or non-linear behavior of the soil that supports them.

## Winkler foundation model
The Winkler foundation model (or Winkler's hypothesis) is a classical mechanical idealization in soil-structure interaction and elasticity theory, introduced by Emil Winkler in 1867.

### Main hypothesis

- Independent Springs: The model represents the supporting soil or foundation medium as a dense system of identical, mutually independent, discrete, linearly elastic springs.
- Local Response: The displacement at any given point on the surface is directly proportional to the normal stress or pressure applied only at that exact point, without any interaction or load-sharing with neighboring springs. 
- Governing Equation: The pressure-deflection relation is expressed as: $p=k \cdot w$ where p is the contact pressure, w is the normal surface deflection (or settlement), and k is the modulus of subgrade reaction (or subgrade spring constant).

### Key Applications

- Beams and Plates on Elastic Foundations: Used to analyze structural members like shallow footings, raft foundations, railway tracks, airport pavements, and laterally loaded piles.
- Soil-Pipe Interaction (SPI): Modeled via Beam-on-Winkler-Foundation (BNWF) approaches using discrete translational springs and dashpots.
- Thin-Film and Elastic Solids Mechanics: Applied to study the buckling, delamination, and adhesion of slender bodies or thin sheets on soft substrates.

### Limitations

- No Shear Continuity: Because the springs are completely isolated, the model ignores shear resistance and shear transfer between adjacent soil regions, meaning it cannot predict continuous "dished" settlement profiles or soil movements at a distance from the loaded zone.
- Difficulty Determining k: The coefficient of subgrade reaction k is not a fundamental soil property; it varies with plate size, foundation geometry, and depth, often requiring empirical correlations (such as plate load tests).

## References
 - [Emil Winkler](https://fr.wikipedia.org/wiki/Emil_Winkler)
