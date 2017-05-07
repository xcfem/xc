# -*- coding: utf-8 -*-

# Impone movimiento nulo en todos los DOF del nodo.
def fixNode6DOF(constraints, idNodo):
  constraints.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  constraints.newSPConstraint(idNodo,1,0.0)
  constraints.newSPConstraint(idNodo,2,0.0)
  constraints.newSPConstraint(idNodo,3,0.0)
  constraints.newSPConstraint(idNodo,4,0.0)
  constraints.newSPConstraint(idNodo,5,0.0)

# Impone movimiento nulo en todos los desplazamientos y los giros de los nodos de la lista.
def fixNode6DOFLista(constraints, tagNodos):
  for i in tagNodos:
    fixNode6DOF(constraints, i)

# Impone movimiento nulo en todos los desplazamientos del nodo.
def Nodo6DOFGirosLibres(constraints, idNodo):
  constraints.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  constraints.newSPConstraint(idNodo,1,0.0)
  constraints.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en todos los desplazamientos de los nodos de la lista.
def Nodo6DOFGirosLibresLista(constraints,tagNodos):
  sz= tagNodos.size
  for i in tagNodos:
    Nodo6DOFGirosLibres(constraints, i)

# # Impone movimiento nulo en todos los desplazamientos del nodo asociado al punto.
# def Nodo6DOFGirosLibresPunto(constraints,double idPunto)
#     tagNod= 
#     \preprocessor
#       {
#         \cad
#           {
#             \pnt[idPunto]
#               {
#                 \set{tagNod= tag_nodo}
#               }
#           }
#         \constraints{\Nodo6DOFGirosLibres(tagNod){}}
#       }

# Impone movimiento nulo en todos los DOF del nodo salvo en el giro respecto al eje y.
def Nodo6DOFGiroYLibre(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0) # Nodo nodeTag
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)
  constraints.newSPConstraint(nodeTag,5,0.0)   

# Impone movimiento nulo en todos los DOF del nodo salvo en el giro respecto al eje z.
def Nodo6DOFGiroZLibre(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0) # Nodo nodeTag
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)
  constraints.newSPConstraint(nodeTag,4,0.0)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro X.
def Nodo6DOFGirosYZLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0) # Nodo nodeTag
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro X
# de los nodos de la lista que se pasa como parámetro.
def Nodo6DOFGirosYZLibresLista(constraints,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosYZLibres(constraints, i)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro Y.
def Nodo6DOFGirosXZLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0) # Nodo nodeTag
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,4,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro Y 
#   de los nodos de la linea que llama a la rutina.}
def Nodo6DOFGirosXZLibresLista(constraints,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosXZLibres(constraints, i)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro Z.
def Nodo6DOFGirosXYLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0) # Nodo nodeTag
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,5,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro Z
#   de los nodos de la linea que llama a la rutina.
def Nodo6DOFGirosXYLibresLinea(constraints,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosXYLibres(constraints, i)

# Impone movimiento nulo en todos los DOF del nodo salvo en el desplazamiento según X y el giro respecto al eje y.
def Nodo6DOFMovXGiroYLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)
  constraints.newSPConstraint(nodeTag,5,0.0)

# Impone movimiento nulo en todos los DOF del nodo salvo en el desplazamiento según X y el giro respecto al eje z.
def Nodo6DOFMovXGiroZLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)
  constraints.newSPConstraint(nodeTag,4,0.0)

# Impone movimiento nulo en los DOF Y y Z y en la rotación según X.
def Nodo6DOFMovXGirosYZLibres(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)
  constraints.newSPConstraint(nodeTag,3,0.0)

# Impone movimiento nulo en los movimientos según X e Y.
def Nodo6DOFMovXYImpedidos(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0)
  constraints.newSPConstraint(nodeTag,1,0.0)

# Impone movimiento nulo en los movimientos según X y Z.
def Nodo6DOFMovXZImpedidos(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)

# Impone movimiento nulo en los movimientos según Y y Z.
def Nodo6DOFMovYZImpedidos(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,1,0.0)
  constraints.newSPConstraint(nodeTag,2,0.0)

# Impone movimiento nulo según X.
def Nodo6DOFMovXImpedido(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,0,0.0)

# Impone movimiento nulo según Y.
def Nodo6DOFMovYImpedido(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,1,0.0)

# Impone movimiento nulo según Z.
def Nodo6DOFMovZImpedido(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,2,0.0)

# Impone giros nulos.
def Nodo6DOFGirosImpedidos(constraints, nodeTag):
  constraints.newSPConstraint(nodeTag,3,0.0)
  constraints.newSPConstraint(nodeTag,4,0.0)
  constraints.newSPConstraint(nodeTag,5,0.0)

def LstNodes6DOFConstr(preprocessor,lstNodes,constrCond):
  '''Constraints the nodes in the list passed as parameter 
  according to given 6-values set of constraints conditions

  :param preprocessor: preprocessor
  :param lstNodes:     list of nodes to which apply the 
                       constraints
  :param constrCond:   list of constraint conditions, expressed as 
               [uX, uY, uZ,rotX, rotY, rotZ], where:

             - uX, uY, uZ: translations in the X, Y and Z directions; 
             - rotX, rotY, rotZ: rotations about the X, Y and Z axis
             - 'free': means no constraint values 
  '''
  constrLoader=preprocessor.getConstraintLoader
  for n in lstNodes:
    for i in range(0,6):
      if(constrCond[i] <> 'free'):
        constrLoader.newSPConstraint(n.tag,i,constrCond[i]) 
