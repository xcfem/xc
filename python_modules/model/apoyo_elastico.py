# -*- coding: utf-8 -*-

# Define un apoyo elástico para el nodo cuyo tag se pasa como parámetro.
def apoyoElasticoXYZ(iNod, iElem, matKX, matKY, matKZ):
  logging.error("apoyoElasticoXYZ obsoleto, emplear defApoyoXYZ.\n")
  return defApoyoXYZ(iNod,iElem,matKx,matKY,matKZ)


# Coloca apoyos elásticos en todos los nodos de la lista (numera los elementos con el mismo índice de los nodos).
def apoyoElasticoNodosLista(tagNodos, tagElementos, matKX, matKY, matKZ):
  logging.error("apoyoElasticoNodosLista obsoleto, emplear defApoyoNodosListaXYZ.\n")
  return defApoyoNodosListaXYZ(tagNodos,tagElementos,matKX,matKY,matKZ)

def apoyoElasticoPuntos(lstPuntosApoyo, offset, matKX, matKY, matKZ):
  logging.error("apoyoElasticoPuntos obsoleto, emplear defApoyoPuntos.\n")
  return defApoyoPuntos(lstPuntosApoyo,offset,matKX,matKY,matKZ)


# Define un apoyo elástico para el nodo cuyo tag se pasa como parámetro.
def apoyoElasticoXYRigZ(iNod, iElem, matKX, matKY):
  logging.error("apoyoElasticoXYRigZ obsoleto, emplear defApoyoXYRigZ.\n")
  return defApoyoXYRigZ(iNod,iElem,matKX,matKY,)

# Define un elemento elástico entre los nodos cuyos tags se pasan como parámetros.
def colocaApoyoElasticoNodos(iNodA, iNodB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  logging.error("colocaApoyoElasticoNodos obsoleto, emplear colocaApoyoEntreNodos.\n")
  return colocaApoyoEntreNodos(iNodA,iNodB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)

# Define un elemento elástico entre los nodos cuyos tags se pasan como parámetros.
def colocaApoyoElasticoXYNodos(iNodA, iNodB, iElem, matKX, matKY):
  logging.error("colocaApoyoElasticoXYNodos obsoleto, emplear colocaApoyoXYEntreNodos.\n")
  return colocaApoyoXYEntreNodos(iNodA,iNodB,iElem,matKX,matKY)

# Define un elemento elástico entre los nodos cuyos tags se pasan como parámetros.
def colocaApoyoElasticoXNodos(iNodA, iNodB, iElem, matKX):
  logging.error("colocaApoyoElasticoXNodos obsoleto, emplear colocaApoyoXEntreNodos.\n")
  return colocaApoyoXEntreNodos(iNodA,iNodB,iElem,matKX)

# Define un elemento elástico entre los nodos cuyos tags se pasan como parámetros.
def colocaApoyoElasticoYNodos(iNodA, iNodB, iElem, matKY):
  logging.error("colocaApoyoElasticoYNodos obsoleto, emplear colocaApoyoYEntreNodos.\n")
  return colocaApoyoYEntreNodos(iNodA,iNodB,iElem,matKY)

# Define un elemento elástico entre los nodos pertenecientes a los puntos cuyos tags se pasan como parámetros.
def colocaApoyoElasticoPuntos(iPtA, iPtB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  logging.error("colocaApoyoElasticoPuntos obsoleto, emplear colocaApoyoEntrePuntos.\n")
  return colocaApoyoEntrePuntos(iPtA,iPtB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)

