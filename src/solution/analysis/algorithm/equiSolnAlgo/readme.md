# Solution algorithms for standard (not eigenvalue) problems


Iteration from the last time step to the current is done by the following methods

    - Linear -- Uses the solution at the first iteration and continues
    - Newton -- Uses the tangent at the current iteration to iterate to convergence
    - Modified Newton -- Uses the tangent at the first iteration to iterate to convergence
	- Krylov-Newton -- Uses a Krylov subspace accelerator to accelerate the convergence of the modified newton method.
	- Newton line search --

## References

- [Variations on Modified Newton](https://portwooddigital.com/2020/11/06/modified-newton/)
- [Better than Ideal Conditions](https://portwooddigital.com/2020/02/08/better-than-ideal-conditions/)
- [Off the Hooke](https://portwooddigital.com/2021/07/15/off-the-hooke/)
- [The Stiffness Matrix Isn’t Everything](https://portwooddigital.com/2022/01/09/the-stiffness-matrix-isnt-everything/)
