# Ill-conditioning in structural finite element models

Finite element models can contain errors that can lead to a class of ill-conditioned stiffness matrices. The extremal eigenpairs of such matrices can be used to identify causes of ill-conditioning.

The verification tests in this folder check the current ill-conditioning detection algorithms in XC.


## Desperately seeking the unconstrained DOF
The most common mistake that leads to a singular stiffness matrix is that the model has an unconstrained degree of freedom (DOF). We are going to review some techniques that can be used to detect this unconstrained degree of freedom.

### Detect floating nodes.
If there is a node that has no constraints at all, i.e. it's not attached to any element or constraint, we can detect it using the `getFloatingNodes()` method of the [`PredefinedSpace`](https://github.com/xcfem/xc/blob/master/python_modules/model/predefined_spaces.py) class. This method returns a list of the nodes that will certainly produce a singular stiffness matrix. If that list is not empty, you can ask the nodes about their identifier (`tag`), their position (`getInitialPos3d`), or any other attribute that could be useful to identify the problem. You can find an example of using this technique in the [`get_floating_nodes_01.py'](https://github.com/xcfem/xc/blob/master/verif/tests/solution/ill_conditioning/get_floating_nodes_01.py) verification test.


