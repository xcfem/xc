# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import puntos as points

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Definimos nodos


puntos= mdlr.getCad.getPoints
pt1= puntos.newPntIDPos3d(1, geom.Pos3d(0.0, 0.0, 0.0))
pt2= puntos.newPntIDPos3d(2, geom.Pos3d(0.0, 1.0, 0.0))
eje= geom.Recta3d(pt1.getPos,pt2.getPos)
ang= math.pi/2.0
trfs= mdlr.getCad.getGeometricTransformations
rot1= trfs.newTransformation("rotation")
rot1.setAxisAng(eje,ang)

pt3= puntos.newPntIDPos3d(3, rot1.getTrfPos(geom.Pos3d(0.0, 0.0, 1.0)))
pt4= puntos.newPntIDPos3d(4, geom.Pos3d(1.0, 0.0, 0.0))

dist34= points.dist(mdlr,3,4)
# print "dist(3,4)= ",dist34

import os
fname= os.path.basename(__file__)
if (dist34<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
