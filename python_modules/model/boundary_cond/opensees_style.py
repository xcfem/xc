# OpenSees style constraints

# Impone movimiento nulo en todos los DOF of the node.
def fix(constraints, idNode,gdls):
  sz= len(gdls)
  for i in range(0,sz):
    if i!=0:
      constraints.newSPConstraint(idNode,i,0.0) # Node id: idNode
