# -*- coding: utf-8 -*-

# Movimientos segun los ejes XY del nodo cuyo código se pasa como parámetro.
def vectorMovUV(idNodo):
  nod= mdlr.getNoadLoader.getNode(idNodo)
  disp= nod.getDisp
  return xc.Vector(disp[0],disp[1])
