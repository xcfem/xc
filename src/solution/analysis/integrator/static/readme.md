Static integrators
==================

Static integrators are incremental integrators provided to implement the
common methods among integrator classes used in performing a static
analysis on the FE\_Model.

Determing the next time step for an analysis can be done by one of the following schemes:

- Load Control -- Specifies the incremental load factor to be applied to the loads in the domain
- Displacement Control -- Specifies the incremental displacement at a specified DOF in the domain
- Minimum Unbalanced Displacement Norm -- Specifies the incremental load factor such that the residual displacement norm in minimized
- Arc Length -- Specifies the incremental arc-length of the load-displacement path
