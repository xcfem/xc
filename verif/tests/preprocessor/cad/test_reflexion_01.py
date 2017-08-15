# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Defining nodes

points= preprocessor.getCad.getPoints
pt1= points.newPntIDPos3d(1, geom.Pos3d(1.0, 0.0, 0.0) )
pt2= points.newPntIDPos3d(2, geom.Pos3d(0.0, 1.0, 0.0) )
pt3= points.newPntIDPos3d(3, geom.Pos3d(0.0, 0.0, 1.0) )
plano= geom.Plane3d(pt1.getPos,pt2.getPos,pt3.getPos)
trfs= preprocessor.getCad.getGeometricTransformations
reflex1= trfs.newTransformation("reflection")
reflex1.setPlane(plano)

pt4= points.newPntIDPos3d(3, reflex1.getTrfPos(geom.Pos3d(2/3.0, 2/3.0, 2/3.0)))

x4= pt4.getPos.x
y4= pt4.getPos.y
z4= pt4.getPos.z

#print "x4= ", x4

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(x4)<1e-12) & (abs(y4)<1e-12) & (abs(z4)<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
