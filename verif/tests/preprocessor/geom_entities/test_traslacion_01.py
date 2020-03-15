# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import model_inquiry as mi

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Defining nodes

pts= preprocessor.getMultiBlockTopology.getPoints
ptA= pts.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,3.0))
ptB= pts.newPntIDPos3d(2,geom.Pos3d(0.0,0.0,4.0))

trfs= preprocessor.getMultiBlockTopology.getGeometricTransformations
transl= trfs.newTransformation("translation")
v= ptB.getPos-ptA.getPos
transl.setVector(v)

ptTrfA= pts.newPntIDPos3d(10,transl.getTrfPos(ptA.getPos))
ptTrfB= pts.newPntIDPos3d(11,transl.getTrfPos(ptB.getPos))

''' 
for p in pts:
  pos= p.getPos
  print "id: ",p.getName," x= ",pos.x,," y= ",pos.y,," z= ",pos.z
 ''' 
  

dist110= mi.distance_2points(preprocessor, 1,10)
# print "dist(1,10)= ",dist110
dist211= mi.distance_2points(preprocessor, 2,11)
# print "dist(2,11)= ",dist211



import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(dist110-1.0)<1e-12) & (abs(dist211-1.0)<1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
