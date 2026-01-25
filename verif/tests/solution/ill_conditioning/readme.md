# Ill-conditioning in structural finite element models

Finite element models can contain errors that can lead to a class of ill-conditioned stiffness matrices. The extremal eigenpairs of such matrices can be used to identify causes of ill-conditioning.

The verification tests in this folder check the current ill-conditioning detection algorithms in XC.


## Desperately seeking the unconstrained DOF
The most common mistake that leads to a singular stiffness matrix is that the model has an unconstrained degree of freedom (DOF). We are going to review some techniques that can be used to detect this unconstrained degree of freedom.

### Detect floating nodes.
If there is a node that has no constraints at all, i.e. it's not attached to any element or constraint, we can detect it using the `getFloatingNodes()` method of the [`PredefinedSpace`](https://github.com/xcfem/xc/blob/master/python_modules/model/predefined_spaces.py) class. This method returns a list of the nodes that will certainly produce a singular stiffness matrix. If that list is not empty, you can ask the nodes about their identifier (`tag`), their position (`getInitialPos3d`), or any other attribute that could be useful to identify the problem. You can find an example of using this technique in the [`get_floating_nodes_01.py'](https://github.com/xcfem/xc/blob/master/verif/tests/solution/ill_conditioning/get_floating_nodes_01.py) verification test.


### Find the unconstrained node from the "zero pivot" row number
Some solvers you can use, store the number of the offending equation when the solver crashes in a property called “info”. This value can be retrieved using the`getProp` method of the solver as follows:

```
result= analysis.analyze(1)  # Run the analysis.

info= None
solver= analysis.linearSOE.solver
if(solver.hasProp("info")):
    info= solver.getProp("info")
```
Once you have the equation number, you can use the `locateEquationNumber` method of `PredefinedSpace` to get the node that has the DOF which corresponds to it:

```
unconstrainedNode= modelSpace.locateEquationNumber(eqNumber= info-1)
```

We subtract 1 from the value of info because C (and C++ and Python) arrays always start at zero, but by default FORTRAN arrays start at 1.

You can find an example of this technique in the [`locate_equation_number_01.py`](https://github.com/xcfem/xc/blob/master/verif/tests/solution/ill_conditioning/locate_equation_number_01.py) verification test.

### Compute the zero energy modes of the stiffness matrix.
When the stiffness matrix is singular, at least one of its eigenvalues is zero. That means that the system offers no resistance to a displacement that follows the corresponding eigenvector.

To compute those eigenvalues and eigenvectors you can use the `zeroEnergyModes` method of the `PredefinedSpace` object:

```
analOk= modelSpace.zeroEnergyModes(1) # Compute one zero energy mode.
eig1= modelSpace.analysis.getEigenvalue(1) # Get the eignvalue.
v1= n1.getEigenvector(1) # eigenvector 1 of node n1
v2= n2.getEigenvector(1) # eigenvector 1 of node n2
```

A trivial example of this technique is use in the [`ill_conditioning_01.py`](https://github.com/xcfem/xc/blob/master/verif/tests/solution/ill_conditioning/ill_conditioning_01.py) verification test. You can also display the eigenvectors to get an indea of what is going on:

```
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

oh.displayEigenvectors()

```

## References

 - [Ill-conditioning in structural finite element models](https://github.com/xcfem/xc/blob/master/verif/tests/solution/ill_conditioning/readme.md)
 - [Like spinning nodes](https://portwooddigital.com/2021/09/19/like-spinning-nodes)
 - [See the Convergence](https://portwooddigital.com/2022/01/24/see-the-convergence/)
 - [Failure to Solve](https://portwooddigital.com/2021/09/12/failure-to-solve)
 - [OpenSees Spy](https://portwooddigital.com/2022/03/13/opensees-spy)
 - [Non-Convergence Is Not Structural Collapse](https://portwooddigital.com/2022/03/20/non-convergence-is-not-structural-collapse/)
 - [Failures detected by LAPACK routines](https://www.netlib.org/lapack/lug/node136.html)
 - [Examining finite element characteristics by eigen modes](http://www.visualfea.com/manual-cbt/chapter/05_02chap.htm)
 - [Invertible Does Not Mean Stable](https://portwooddigital.com/2024/12/11/invertible-does-not-mean-stable/)
 - [Reverse Engineering the Equation Numberer](https://portwooddigital.com/2026/01/06/reverse-engineering-the-equation-numberer/)
 - [Failed to Get Compatible …](https://portwooddigital.com/2021/10/10/failed-to-get-compatible/)
