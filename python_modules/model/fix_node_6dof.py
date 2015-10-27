# Impone movimiento nulo en todos los DOF del nodo.
def fixNode6DOF(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,4,0.0)
  coacciones.newSPConstraint(idNodo,5,0.0)

# Impone movimiento nulo en todos los desplazamientos y los giros de los nodos de la lista.
def fixNode6DOFLista(coacciones, tagNodos):
  for i in tagNodos:
    fixNode6DOF(coacciones, i)

# Impone movimiento nulo en todos los desplazamientos del nodo.
def Nodo6DOFGirosLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en todos los desplazamientos de los nodos de la lista.
def Nodo6DOFGirosLibresLista(coacciones,tagNodos):
  sz= tagNodos.size
  for i in tagNodos:
    Nodo6DOFGirosLibres(coacciones, i)

# # Impone movimiento nulo en todos los desplazamientos del nodo asociado al punto.
# def Nodo6DOFGirosLibresPunto(coacciones,double idPunto)
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
def Nodo6DOFGiroYLibre(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,5,0.0)   

# Impone movimiento nulo en todos los DOF del nodo salvo en el giro respecto al eje z.
def Nodo6DOFGiroZLibre(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,4,0.0)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro X.
def Nodo6DOFGirosYZLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro X
# de los nodos de la lista que se pasa como parámetro.
def Nodo6DOFGirosYZLibresLista(coacciones,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosYZLibres(coacciones, i)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro Y.
def Nodo6DOFGirosXZLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,4,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro Y 
#   de los nodos de la linea que llama a la rutina.}
def Nodo6DOFGirosXZLibresLista(coacciones,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosXZLibres(coacciones, i)

# Impone movimiento nulo en todos los desplazamientos del nodo y en el giro Z.
def Nodo6DOFGirosXYLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0) # Nodo idNodo
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,5,0.0)

# Impone movimiento nulo en todos los desplazamientos y en el giro Z
#   de los nodos de la linea que llama a la rutina.
def Nodo6DOFGirosXYLibresLinea(coacciones,tagNodos):
  for i in tagNodos:
    Nodo6DOFGirosXYLibres(coacciones, i)

# Impone movimiento nulo en todos los DOF del nodo salvo en el desplazamiento según X y el giro respecto al eje y.
def Nodo6DOFMovXGiroYLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,5,0.0)

# Impone movimiento nulo en todos los DOF del nodo salvo en el desplazamiento según X y el giro respecto al eje z.
def Nodo6DOFMovXGiroZLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,4,0.0)

# Impone movimiento nulo en los DOF Y y Z y en la rotación según X.
def Nodo6DOFMovXGirosYZLibres(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)
  coacciones.newSPConstraint(idNodo,3,0.0)

# Impone movimiento nulo en los movimientos según X e Y.
def Nodo6DOFMovXYImpedidos(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0)
  coacciones.newSPConstraint(idNodo,1,0.0)

# Impone movimiento nulo en los movimientos según X y Z.
def Nodo6DOFMovXZImpedidos(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en los movimientos según Y y Z.
def Nodo6DOFMovYZImpedidos(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo según X.
def Nodo6DOFMovXImpedido(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,0,0.0)

# Impone movimiento nulo según Y.
def Nodo6DOFMovYImpedido(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,1,0.0)

# Impone movimiento nulo según Z.
def Nodo6DOFMovZImpedido(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,2,0.0)

# Impone movimiento nulo en los giros.
def Nodo6DOFGirosImpedidos(coacciones, idNodo):
  coacciones.newSPConstraint(idNodo,3,0.0)
  coacciones.newSPConstraint(idNodo,4,0.0)
  coacciones.newSPConstraint(idNodo,5,0.0)

