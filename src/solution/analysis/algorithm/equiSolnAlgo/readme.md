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
### Krylov space methods
- [Krylov subspace](https://en.wikipedia.org/wiki/Krylov_subspace)
- [A Brief Introduction to Krylov Space Methods for Solving Linear Systems](https://people.math.ethz.ch/~mhg/pub/biksm.pdf)
- [A Krylov Subspace Accelerated Newton Algorithm](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=49d46f2df3420b3689212a24ce7e09f609e4a26b)
- [Secant accelerated Newton Algorithm](https://portwooddigital.com/2024/08/18/secant-accelerated-newton-algorithm)
