# Pre-processing
This directory contanins scripts to facilitate the building of the model.

It's organized in 4 groups:
    -geometry: utilities to create the model geometry (points, lines,
               surfaces, ...)
    -mesh: utilitiess to mesh the model (sections, nodes, elements)
    -boundary_cond: utilitiess to define boundary conditions
    -sets: utilitiess to manage sets of entities, select options, ...

## Future enhancement

Implement a joint finder as follows:

 - Iterate through the nodes of the model.
 - If the node is connected to one element we have a "dangling" node.
 - If the node is connected to two elements or more:
     - If we have two elements and they are aligned (angle<tol), there is no joint.
	 - If the angle between the two elements is greater than the tolerance we have a joint.
	 - The displacement of the node is prevented in the direction of the connected element.
	 - If the connected elements are contained in a plane we have a 2D joint if they are contained in two or more planes we have a 3D joints.
	 
Implement a member finder as follows:

 - Each member is formed by a sequence of elements between two 3D joints or a 3D joint and a dangling node.
 - The buckling lengths of the member are defined by the directions of the elements connected along the member as seen by the member elements (local axis). 

