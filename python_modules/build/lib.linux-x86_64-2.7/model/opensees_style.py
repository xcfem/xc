# OpenSees style constraints

# Impone movimiento nulo en todos los DOF del nodo.
def fix(coacciones, idNodo,gdls):
  sz= len(gdls)
  for i in range(0,sz):
    if i!=0:
      coacciones.newSPConstraint(idNodo,i,0.0) # Nodo idNodo
