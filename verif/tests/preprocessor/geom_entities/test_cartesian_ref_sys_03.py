# -*- coding: utf-8 -*-
from __future__ import print_function

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

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntIDPos3d(1,geom.Pos3d(1.0,1.0,0.0))
pt2= points.newPntIDPos3d(2,geom.Pos3d(2,2,0))
pt3= points.newPntIDPos3d(3,geom.Pos3d(0,1,0))

rs= geom.Ref3d3d(pt1.getPos,pt2.getPos,pt3.getPos)

p= rs.getPosGlobal(geom.Pos3d(7, 0, 0))

nodes= preprocessor.getNodeHandler
nod1= nodes.newNodeXYZ(p.x,p.y,p.z)
coord= nod1.getCoo

ratio1= (coord[0]-5.9497475)
ratio2= coord[1]-5.9497475
ratio3= coord[2]


''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
 '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
