# Convenience classes and functions to solve the FE problem.

Once the analyst has defined the model, the next step is to define the analysis that is to be performed on the model. This may vary from a simple static linear analysis to a transient non-linear analysis.

## References

 - [What is a FEA Solver?](https://feaforall.com/what-is-an-fea-solver)
 - [Like spinning nodes](https://portwooddigital.com/2021/09/19/like-spinning-nodes)
 - [Not All at Once](https://portwooddigital.com/2021/10/26/not-all-at-once/)
 - [Finite Differences](https://portwooddigital.com/2021/11/27/finite-differences/)
 - [See the Convergence](https://portwooddigital.com/2022/01/24/see-the-convergence/)
 - [Don’t Invert the Matrix](https://portwooddigital.com/2022/11/18/dont-invert-the-matrix/)
 - [How to Profile an OpenSeesPy Analysis](https://portwooddigital.com/2024/12/22/how-to-profile-an-openseespy-analysis/)
 
### Ill conditioning

 - [Like spinning nodes](https://portwooddigital.com/2021/09/19/like-spinning-nodes)
 - [See the Convergence](https://portwooddigital.com/2022/01/24/see-the-convergence/)
 - [Failure to Solve](https://portwooddigital.com/2021/09/12/failure-to-solve)
 - [OpenSees Spy](https://portwooddigital.com/2022/03/13/opensees-spy)
 - [Non-Convergence Is Not Structural Collapse](https://portwooddigital.com/2022/03/20/non-convergence-is-not-structural-collapse/)
 - [Failures detected by LAPACK routines](https://www.netlib.org/lapack/lug/node136.html)
 - [Examining finite element characteristics by eigen modes](http://www.visualfea.com/manual-cbt/chapter/05_02chap.htm)
 - [Invertible Does Not Mean Stable](https://portwooddigital.com/2024/12/11/invertible-does-not-mean-stable/)
- [A Model of Inconsistency](https://portwooddigital.com/2025/08/10/a-model-of-inconsistency/)

### Constraint handling
- [Handling a Doubt](https://portwooddigital.com/2023/11/09/handling-a-doubt/)
- [What Is a Good Penalty Number?](https://portwooddigital.com/2023/11/29/what-is-a-good-penalty-number/)
- [Daisy Chains and Gaffes](https://portwooddigital.com/2025/10/26/daisy-chains-and-gaffes/)

### Static analysis

- [Static Analysis with Uniform Excitation](https://portwooddigital.com/2024/07/28/static-analysis-with-uniform-excitation/)

### Modal analysis
- [Modal analysis of rigid multy-storey frames](https://structville.com/2016/04/on-modal-analysis-of-rigid-multi-storey-frames.html)
- [All About Making that ZPA](https://portwooddigital.com/2024/10/06/all-about-making-that-zpa/)

### Time-domain transient analysis
- [OpenSeesaw](https://portwooddigital.com/2021/11/12/openseesaw/)
- [Structural dynamics](https://en.wikipedia.org/wiki/Structural_dynamics)
- [Initial conditions](https://portwooddigital.com/2021/11/20/initial-conditions/)
- [OpenSees: Static, Cyclic, and Dynamic Analysis Examples](https://opensees.berkeley.edu/workshop/OpenSeesDays2008/A10_UsersExampleAnalysis.pdf)
- [Transient Analysis of a Cantilever Beam](https://sites.ualberta.ca/~wmoussa/AnsysTutorial/IT/Transient/Transient.html)
- [The time-domain transient method in structural finite element analysis](https://getwelsim.medium.com/the-time-domain-transient-method-in-structural-finite-element-analysis-3666dd066d81)
- [Newmark method](https://opensees.github.io/OpenSeesDocumentation/user/manual/analysis/Newmark.html)
- [Newmark method](https://opensees.berkeley.edu/wiki/index.php/Newmark_Method)
- [Newmark-beta method](https://en.wikipedia.org/wiki/Newmark-beta_method)
- [No Element Required](https://portwooddigital.com/2024/01/14/no-element-required/)
- [Two Sprung Masses and Some Friction Force](https://portwooddigital.com/2024/02/10/two-sprung-masses-and-some-friction-force/)
- [A Simple Base-Isolated Model](https://portwooddigital.com/2024/11/26/a-simple-base-isolated-model/)
- [Minimal Damper Example](https://portwooddigital.com/2025/01/12/minimal-damper-example/)
- [Minimal Random Process Example](https://portwooddigital.com/2025/09/28/minimal-random-process-example/)


### Eigenvalue analysis
 - [Ordinary Eigenvalues](https://portwooddigital.com/2020/11/13/ordinary-eigenvalues/)
 - [Eigenvalues During an Analysis](https://portwooddigital.com/2021/11/09/eigenvalues-during-an-analysis/)
 - [More Than You or Eigen Chew](https://portwooddigital.com/2022/03/06/more-than-you-or-eigen-chew/)
 - [Eigenvalues and eigenvectors](https://en.wikipedia.org/wiki/Eigenvalues_and_eigenvectors)
 - [Eigenvalue algorithm](https://en.wikipedia.org/wiki/Eigenvalue_algorithm)
 - [Ordinary Eigenvalues](https://portwooddigital.com/2020/11/13/ordinary-eigenvalues/)
 - [One Way to Get Bad Eigenvalues](https://portwooddigital.com/2022/11/10/one-way-to-get-bad-eigenvalues/)
 - [Another Way to Get Bad Eigenvalues](https://portwooddigital.com/2022/11/11/another-way-to-get-bad-eigenvalues/)
 - [Quick (and Dirty) Modal Damping](https://portwooddigital.com/2022/11/08/quick-and-dirty-modal-damping/)
 - [Eigenvalues of the Stiffness Matrix](https://portwooddigital.com/2023/01/16/eigenvalues-of-the-stiffness-matrix/)
 - [Spectra. C++ Library For Large Scale Eigenvalue Problems](https://spectralib.org/)
 - [Eigen Almost Hear You Sigh](https://portwooddigital.com/2025/12/30/eigen-almost-hear-you-sigh/)

#### Linear buckling analysis.
- [Buckling](https://en.wikipedia.org/wiki/Buckling)
- [Linear buckling in plain language!](https://enterfea.com/linear-buckling-explained/)
- [Linear buckling analysis](https://portwooddigital.com/2021/05/29/right-under-your-nose)
- [Minimal Plate Buckling Example](https://portwooddigital.com/2025/05/24/minimal-plate-buckling-example/)
- [Buckling of Restrained Plates](https://portwooddigital.com/2025/05/31/buckling-of-restrained-plates/)

### Arc-length technique
- [Arc-length technique for nonlinear finite element analysis](https://www.researchgate.net/publication/8618852_Arc-length_technique_for_nonlinear_finite_element_analysis)
- [Arc-Length Control](https://opensees.berkeley.edu/OpenSees/manuals/usermanual/684.htm)
- [Arc Length Parameters](https://portwooddigital.com/2023/05/02/arc-length-parameters/)

### Root-finding algorithms
- [Variations on Modified Newton](https://portwooddigital.com/2020/11/06/modified-newton/)
- [Better than Ideal Conditions](https://portwooddigital.com/2020/02/08/better-than-ideal-conditions/)
- [Off the Hooke](https://portwooddigital.com/2021/07/15/off-the-hooke/)
- [The Stiffness Matrix Isn’t Everything](https://portwooddigital.com/2022/01/09/the-stiffness-matrix-isnt-everything/)
#### Krylov space methods
- [Krylov subspace](https://en.wikipedia.org/wiki/Krylov_subspace)
- [A Brief Introduction to Krylov Space Methods for Solving Linear Systems](https://people.math.ethz.ch/~mhg/pub/biksm.pdf)
- [A Krylov Subspace Accelerated Newton Algorithm](https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=49d46f2df3420b3689212a24ce7e09f609e4a26b)
- [Secant accelerated Newton Algorithm](https://portwooddigital.com/2024/08/18/secant-accelerated-newton-algorithm)

# Equation numberer
- [Reverse Engineering the Equation Numberer](https://portwooddigital.com/2026/01/06/reverse-engineering-the-equation-numberer/)
