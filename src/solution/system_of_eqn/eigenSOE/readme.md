# Eigenvalue Solvers

Eigenvalues frequently appear in structural analysis. The most common cases are vibration frequencies and eigenvalues in the form of load magnitudes in structural stability analysis.

In linear algebra, an eigenvector or characteristic vector of a linear transformation is a nonzero vector that changes at most by a scalar factor when that linear transformation is applied to it. The corresponding eigenvalue, often denoted by $\lambda$, is the factor by which the eigenvector is scaled.

Geometrically, an eigenvector, corresponding to a real nonzero eigenvalue, points in a direction in which it is stretched by the transformation and the eigenvalue is the factor by which it is stretched. If the eigenvalue is negative, the direction is reversed. Loosely speaking, in a multidimensional vector space, the eigenvector is not rotated. 

The following methods provide the solution of the generalized eigenvalue problem Kv = MvL

    -Symmetric Arpack -- Arpack solver for symmetric matrices.
    -Band Arpack -- Arpack solver for banded matrices.
	-Full general -- Solver for dense matrix.
	
## References
 - [Linear buckling analysis](https://portwooddigital.com/2021/05/29/right-under-your-nose)
 - [Eigenvalues During an Analysis](https://portwooddigital.com/2021/11/09/eigenvalues-during-an-analysis/)
 - [More Than You or Eigen Chew](https://portwooddigital.com/2022/03/06/more-than-you-or-eigen-chew/)
 - [Eigenvalues and eigenvectors](https://en.wikipedia.org/wiki/Eigenvalues_and_eigenvectors)
 - [Eigenvalue algorithm](https://en.wikipedia.org/wiki/Eigenvalue_algorithm)
