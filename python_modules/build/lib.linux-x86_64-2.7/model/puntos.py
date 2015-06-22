# -*- coding: utf-8 -*-
# Utilidades relacionadas con los puntos

# Devuelve el identificador del nodo asociado al punto.
def getTagNodoPunto(mdlr,idPunto):
  return mdlr.getCad.getPoints.get(idPunto).getTagNode


# Devuelve el identificador del nodo asociado al punto.
def getTagNodosPuntos(lstPuntos):
  retva=[] 
  for p in lstPuntos:
    retval.append(p.getTagNodo())
  return retval

def dist(mdlr,a,b):
  puntos= mdlr.getCad.getPoints
  pA= puntos.get(a).getPos
  pB= puntos.get(b).getPos
  return pA.distPos3d(pB)
