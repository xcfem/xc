# -*- coding: utf-8 -*-
import xc_base
import geom
import xc

# Movimientos segun los ejes XYZ del nodo cuyo código se pasa como parámetro.
def vectorMovUVW(mdlr,idNodo):
  nod= mdlr.getNodeLoader.getNode(idNodo)
  disp= nod.getDisp
  return xc.Vector([disp[0],disp[1],disp[2]])
