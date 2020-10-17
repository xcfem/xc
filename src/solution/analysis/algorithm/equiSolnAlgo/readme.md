Solution algorithms for standard (not eigenvalue) problems
==========================================================

Iteration from the last time step to the current is done by the following methods

    - Linear -- Uses the solution at the first iteration and continues
    - Newton -- Uses the tangent at the current iteration to iterate to convergence
    - Modified Newton -- Uses the tangent at the first iteration to iterate to convergence
	- Krylov-Newton -- Uses a Krylov subspace accelerator to accelerate the convergence of the modified newton method.
	- Newton line search --
