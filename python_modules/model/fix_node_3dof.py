# -*- coding: utf-8 -*-


# Impone movimiento nulo en todos los GDL del nodo.
def fixNode3DOF(coacciones, idNodo):
  print "obsolete, use fixNode000."
  fixNode000(coacciones, idNodo)

# UX= 0, UY= 0, THETA= 0
def fixNode000(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en todos los GDL del nodo salvo en el giro.
def Nodo3GDLGiroLibre(coacciones, idNodo):
  print "obsolete use fixNode00F"
  fixNode00F(coacciones,idNodo)

# UX= 0, UY= 0
def fixNode00F(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)

# UX= 0, UY= 0
def fixNode0FF(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo

# UY= 0
def fixNodeF0F(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)


# Impone movimiento nulo en los giros.
def Nodo3GDLGirosImpedidos(coacciones, idNodo):
  print "obsolete use fixNodeFF0"
  coacciones.newSPConstraint(idNodo,2,0.0)


# THETA= 0
def fixNodeFF0(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en todos los nodos de 
# la línea cuyo nombre se pasa como parámetro.}
def fijaNodosLinea3GDL(constraints, linea):
  nn= linea.getNumNodes
  for i in range(1,nn+1):
    tagNodo= linea.getNodeI(i).tag
    fixNode000(constraints,tagNodo)
