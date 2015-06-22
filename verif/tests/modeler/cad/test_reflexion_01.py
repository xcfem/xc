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
pt1= puntos.newPntIDPos3d(1, geom.Pos3d(1.0, 0.0, 0.0) )
pt2= puntos.newPntIDPos3d(2, geom.Pos3d(0.0, 1.0, 0.0) )
pt3= puntos.newPntIDPos3d(3, geom.Pos3d(0.0, 0.0, 1.0) )
plano= geom.Plano3d(pt1.getPos,pt2.getPos,pt3.getPos)
trfs= mdlr.getCad.getGeometricTransformations
reflex1= trfs.newTransformation("reflection")
reflex1.setPlane(plano)

pt4= puntos.newPntIDPos3d(3, reflex1.getTrfPos(geom.Pos3d(2/3.0, 2/3.0, 2/3.0)))

x4= pt4.getPos.x
y4= pt4.getPos.y
z4= pt4.getPos.z

#print "x4= ", x4

import os
fname= os.path.basename(__file__)
if (abs(x4)<1e-12) & (abs(y4)<1e-12) & (abs(z4)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
