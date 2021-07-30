# Analysis components

The Analysis object is responsible for performing the analysis. In XC (as in OpenSees) each Analysis object is composed of several component objects, which define how the analysis is performed. The component classes consist of the following: 

The components of the analysis are: 

- Solution algorithm: object that find the solution from the last time step to the current one. There are algorithms for eigenvalue problems (standard, linear buckling, frequencies,...) and for linear and non-linear systems of equations (Newton-Raphson, Line search, Krylov-Newton, Linear,...). There are also domain decomposition algorithms.
- Model wrapper: wrapper of the finite element model "as seen" by the analysis.
- Integrator: form the mass and stiffness matrices from the domain components.
- Constraint handler: algorithm used to introduce the problem constraints in the solution algorithm (plain, penalty, lagrange multipliers, transformation). 
- Convergence tests: accepting the current state of the domain as being on the converged solution path is accomplished by the convergence tests.
- DOF numberer: This object determines the mapping between equation numbers and degrees-of-freedom -- how degrees-of-freedom are numbered.
- System of equations/solver:  within the solution algorithm, it specifies how to store and solve the system of equations in the analysis.

## Contents

        - algorithm: SolutionAlgorithm class and 2 subdirectories:
                equiSolnAlgo: Linear, NewtonRaphson and ModifiedNewton
                domainDecompAlgo: DomainDecompAlgo
        - analysis: the Analysis, StaticAnalysis, and some others
        - handler: ConstraintHandler, PenaltyConstraintHandler and
                PlainHandler
        - integrator: Integrator, IncrementalIntegrator, LoadControl,
                ArcLength, StaticIntegrator, TransientIntegrator and
                Newmark
        - model: AnalysisModel and its iterators.
        - numberer: DOF_Numberer

## References

- [Its power and its problem](https://portwooddigital.com/2021/04/18/its-power-and-its-problem/)
- [Numerical Linear Algebra Problems in StructuralAnalysis](http://eprints.maths.manchester.ac.uk/2195/1/thesis-main.pdf)
