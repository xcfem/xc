Analysis components
===================

Components of the analysis: model wrapper, integrators, solution algorithms, convergence tests and constraint handlers.

- Model wrapper: wrapper of the finite element model "as seen" by the analysis.
- Solution algorithms: objects that find the solution from the last time step to the current one. There are algorithms for eigenvalue problems (standard, linear buckling, frequencies,...) and for linear and non-linear systems of equations (Newton-Raphson, Line search, Krylov-Newton, Linear,...). There are also domain decomposition algorithms.
. Convergence tests: accepting the current state of the domain as being on the converged solution path is accomplished by the convergence tests.
