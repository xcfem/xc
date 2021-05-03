# Beam with hinges element

This command is used to construct a forceBeamColumn element object, which is based on the non-iterative (or iterative) flexibility formulation. The locations and weights of the element integration points are based on so-called plastic hinge integration, which allows the user to specify plastic hinge lenghts at the element ends. Two-point Gauss integration is used on the element interior while two-point Gauss-Radau integration is applied over lengths of 4LpI and 4LpJ at the element ends, viz. "modified Gauss-Radau plastic hinge integration".



[Release the plastic hinge](https://portwooddigital.com/2021/05/02/release-the-plastic-hinge/)
