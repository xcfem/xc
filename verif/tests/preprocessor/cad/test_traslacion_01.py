# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import puntos

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos nodos

pts= preprocessor.getCad.getPoints
ptA= pts.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,3.0))
ptB= pts.newPntIDPos3d(2,geom.Pos3d(0.0,0.0,4.0))

trfs= preprocessor.getCad.getGeometricTransformations
transl= trfs.newTransformation("translation")
v= ptB.getPos-ptA.getPos
transl.setVector(v)

ptTrfA= pts.newPntIDPos3d(10,transl.getTrfPos(ptA.getPos))
ptTrfB= pts.newPntIDPos3d(11,transl.getTrfPos(ptB.getPos))

''' 
for p in pts:
  pos= p.getPos
  print "id: ",p.getNombre," x= ",pos.x,," y= ",pos.y,," z= ",pos.z
 ''' 
  

dist110= puntos.dist(preprocessor, 1,10)
# print "dist(1,10)= ",dist110
dist211= puntos.dist(preprocessor, 2,11)
# print "dist(2,11)= ",dist211



import os
fname= os.path.basename(__file__)
if (abs(dist110-1.0)<1e-12) & (abs(dist211-1.0)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
