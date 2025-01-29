# Constraint

The classes in this folder represent the degrees of freedom that are somewhat restrained. Constraints enhance computational efficiency by reducing the number of equations necessary for solution (unless a Lagrange constraints handler is used).

## Single freedom constraints

A single freedom constraint specifies the response of a particular degree-of-freedom at a node. This type of constraints are used to represent:

- Support points (zero displacement along one or more degrees of freedom).
- Support settlement (nonzero displacement along one or more degrees of freedom).
- Imposed motions (displacement varying with time along one or more degrees of freedom).


## Multifreedom constraints

A multifreedom constraint imposes a relationship between the displacement for certain dof at two nodes in the model, typically called the _retained node_ and the _constrained node_: $U_c = C_{cr} U_r$

Multifreedom may be used to model:

- rigid-body behavior, in which joints translate and rotate together.
- equal-displacement behavior, in which displacements along certain degrees of freedom are equal

### Single node multifreedom constraints
A multi-freedom constraint is called singlepoint or singlenode if it involves displacement components at one node onlys.

#### Skew multifreedom constraints
Multifreedom constraints where both the retained, and the constrained node are the same.

### Multipoint of multinode multifreedom constraints
A multi-freedom constraint is called multipoint or multinode if it involves displacement components at different nodes.


## References
 - [Support and Connection Types](https://web.mit.edu/4.441/1_lectures/1_lecture13/1_lecture13.html)
 - [Handling of Constraints in Finite-Element Response Sensitivity Analysis](http://jaguar.ucsd.edu/sensitivity/Gu_2009_b.pdf)
 - [Rigid Diaphragm for 2D Models](https://portwooddigital.com/2022/11/05/rigid-diaphragm-for-2d-models/)
 - [Handle Your Constraints with Care](https://portwooddigital.com/2020/06/05/handle-your-constraints-with-care/)
 - [A Rigid Bar Walks Into a Bar](https://portwooddigital.com/2023/04/02/a-rigid-bar-walks-into-a-bar/)
- [Handling a Doubt](https://portwooddigital.com/2023/11/09/handling-a-doubt/)
- [What Is a Good Penalty Number?](https://portwooddigital.com/2023/11/29/what-is-a-good-penalty-number/)
- [Constraint Matrix Not Identity](https://portwooddigital.com/2023/11/07/constraint-matrix-not-identity/)
- [Finite Element Procedures, section 4.2](https://books.google.es/books/about/Finite_Element_Procedures.html?id=rWvefGICfO8C&redir_esc=y)
