# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import puntos as points

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos nodos


puntos= preprocessor.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(0,0,0))
pt2= puntos.newPntIDPos3d(2,geom.Pos3d(0,0,1))
pt3= puntos.newPntIDPos3d(3,geom.Pos3d(1,0,0))

eje= geom.Recta3d(pt1.getPos,pt2.getPos)
ang= math.pi/2.0
trfs= preprocessor.getCad.getGeometricTransformations
rot1= trfs.newTransformation("rotation")
rot1.setAxisAng(eje,-ang)

pt4= puntos.newPntIDPos3d(4,rot1.getTrfPos(geom.Pos3d(0.0, 1.0, 0.0)))

dist34= points.dist(preprocessor, 3,4)
#print "dist(3,4)= ",dist34

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (dist34<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
