# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

# Define un apoyo elastico para el nodo cuyo tag se pasa como parametro.
def apoyoElasticoXYZ(iNod, iElem, matKX, matKY, matKZ):
  lmsg.error("apoyoElasticoXYZ obsoleto, emplear defApoyoXYZ.\n")
  return defApoyoXYZ(iNod,iElem,matKx,matKY,matKZ)


# Coloca apoyos elasticos en todos los nodos de la lista (numera los elementos con el mismo índice de los nodos).
def apoyoElasticoNodosLista(tagNodos, tagElementos, matKX, matKY, matKZ):
  lmsg.error("apoyoElasticoNodosLista obsoleto, emplear defApoyoNodosListaXYZ.\n")
  return defApoyoNodosListaXYZ(tagNodos,tagElementos,matKX,matKY,matKZ)

def apoyoElasticoPuntos(lstPuntosApoyo, offset, matKX, matKY, matKZ):
  lmsg.error("apoyoElasticoPuntos obsoleto, emplear defApoyoPuntos.\n")
  return defApoyoPuntos(lstPuntosApoyo,offset,matKX,matKY,matKZ)


# Define un apoyo elastico para el nodo cuyo tag se pasa como parametro.
def apoyoElasticoXYRigZ(iNod, iElem, matKX, matKY):
  lmsg.error("apoyoElasticoXYRigZ obsoleto, emplear defApoyoXYRigZ.\n")
  return defApoyoXYRigZ(iNod,iElem,matKX,matKY,)

# Define un elemento elastico entre los nodos cuyos tags se pasan como parametros.
def colocaApoyoElasticoNodos(iNodA, iNodB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  lmsg.error("colocaApoyoElasticoNodos obsoleto, emplear colocaApoyoEntreNodos.\n")
  return colocaApoyoEntreNodos(iNodA,iNodB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)

# Define un elemento elastico entre los nodos cuyos tags se pasan como parametros.
def colocaApoyoElasticoXYNodos(iNodA, iNodB, iElem, matKX, matKY):
  lmsg.error("colocaApoyoElasticoXYNodos obsoleto, emplear colocaApoyoXYEntreNodos.\n")
  return colocaApoyoXYEntreNodos(iNodA,iNodB,iElem,matKX,matKY)

# Define un elemento elastico entre los nodos cuyos tags se pasan como parametros.
def colocaApoyoElasticoXNodos(iNodA, iNodB, iElem, matKX):
  lmsg.error("colocaApoyoElasticoXNodos obsoleto, emplear colocaApoyoXEntreNodos.\n")
  return colocaApoyoXEntreNodos(iNodA,iNodB,iElem,matKX)

# Define un elemento elastico entre los nodos cuyos tags se pasan como parametros.
def colocaApoyoElasticoYNodos(iNodA, iNodB, iElem, matKY):
  lmsg.error("colocaApoyoElasticoYNodos obsoleto, emplear colocaApoyoYEntreNodos.\n")
  return colocaApoyoYEntreNodos(iNodA,iNodB,iElem,matKY)

# Define un elemento elastico entre los nodos pertenecientes a los puntos cuyos tags se pasan como parametros.
def colocaApoyoElasticoPuntos(iPtA, iPtB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  lmsg.error("colocaApoyoElasticoPuntos obsoleto, emplear colocaApoyoEntrePuntos.\n")
  return colocaApoyoEntrePuntos(iPtA,iPtB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)

