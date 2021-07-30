# Constraint handlers


The ConstraintHandler object determines how the constraint equations are enforced in the analysis. Constraint equations enforce a specified value for a DOF, or a relationship between DOFs.

The available constraint handlers are:

- Plain Constraints: Removes constrained degrees of freedom from the system of equations. A plain constraint handler can only enforce homogeneous single point constraints (fix command) and multi-point constraints constructed where the constraint matrix is equal to the identity (equalDOF command).
- Lagrange Multipliers: Uses the method of Lagrange multipliers to enforce constraints. A LagrangeMultiplier constraint handler enforces the constraints by introducing Lagrange multipliers to the system of equations. The Lagrange multiplier method introduces new unknowns to the system of equations. The diagonal part of the system corresponding to these new unknowns is 0.0. This ensure that the system IS NOT symmetric positive definite.
- Penalty Method:  Uses penalty numbers to enforce constraints. A penalty constraint handler enforces the constraints using the penalty method. The degree to which the constraints are enforced is dependent on the penalty values chosen. Problems can arise if these values are too small (constraint not enforced strongly enough) or too large (problems associated with conditioning of the system of equations).
- Transformation Method: Performs a condensation of constrained degrees of freedom. A transformation constraint handler enforces the constraints using the transformation method.
    * The single-point constraints when using the transformation method are done directly. The matrix equation is not manipulated to enforce them, rather the trial displacements are set directly at the nodes at the start of each analysis step.
	* Great care must be taken when multiple constraints are being enforced as the transformation method does not follow constraints:
	    - If a node is fixed, constrain it with the fix command and not equalDOF or other type of constraint.
		- If multiple nodes are constrained, make sure that the retained node is not constrained in any other constraint.
	* And remember if a node is constrained to multiple nodes in your model it probably means you have messed up. 

