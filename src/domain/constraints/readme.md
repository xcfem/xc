# Constraint

The classes in this folder represent the degrees of freedom that are somewhat restrained. Constraints enhance computational efficiency by reducing the number of equations necessary for solution (unless a Lagrange constraints handler is used).

## Single freedom constraints

A single freedom constraint specifies the response of a particular degree-of-freedom at a node. This type of constraints are used to represent:

- Support points (zero displacement along one or more degrees of freedom).
- Support settlement (nonzero displacement along one or more degrees of freedom).
- Imposed motions (displacement varying with time along one or more degrees of freedom).


## Multi-freedom constraints

A multifreedom constraint imposes a relationship between the displacement for certain dof at two nodes in the model, typically called the _retained node_ and the _constrained node_: $U_c = C_{cr} U_r$

Multi-freedom may be used to model:

- rigid-body behavior, in which joints translate and rotate together.
- equal-displacement behavior, in which displacements along certain degrees of freedom are equal

