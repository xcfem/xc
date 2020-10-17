Solution of the linear system of equations
==========================================

The following methods provide the solution of the linear system of equations Ku = P. Each solver is tailored to a specific matrix topology. All of them have been inherited from OpenSees.

    - Profile SPD -- Direct profile solver for symmetric positive definite matrices
    - Band General -- Direct solver for banded unsymmetric matrices
    - Band SPD -- Direct solver for banded symmetric positive definite matrices
    - Sparse General -- Direct solver for unsymmetric sparse matrices
    - Sparse Symmetric -- Direct solver for symmetric sparse matrices
    - UmfPack General -- Direct UmfPack solver for unsymmetric matrices
    - Full General -- Direct solver for unsymmetric dense matrices
    - Conjugate Gradient -- Iterative solver using the preconditioned conjugate gradient method
