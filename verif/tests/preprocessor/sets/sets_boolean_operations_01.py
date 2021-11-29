# -*- coding: utf-8 -*-
from __future__ import print_function
CooMax= 10
import xc_base
import geom
import xc
import math
from model.sets import sets_mng as sUtils

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(2,geom.Pos3d(CooMax/2,CooMax/2,CooMax/2))
pt3= points.newPoint(3,geom.Pos3d(CooMax,CooMax,CooMax))

s1= preprocessor.getSets.defSet("S1")
sUtils.append_points(s1,[pt1,pt2])

s2= preprocessor.getSets.defSet("S2")
sUtils.append_points(s2,[pt2,pt3])

s3= preprocessor.getSets.defSet("S3")
s3+= s1
s3+= s2
sz3= s3.getPoints.size

s4= preprocessor.getSets.defSet("S4")
s4+= s1
s4-= s2
sz4= s4.getPoints.size

s5= preprocessor.getSets.defSet("S5")
s5+= s1
s5*= s2
sz5= s5.getPoints.size

#for p in pnts:
#  print(code)

#print("sz= ", sz)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (sz3==3) and (sz4==1) and (sz5==1):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
