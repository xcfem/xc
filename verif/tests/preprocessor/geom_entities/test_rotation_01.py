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
preprocessor=  feProblem.getPreprocessor
# Defining nodes


points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(0,0,1))
pt3= points.newPoint(geom.Pos3d(1,0,0))

axis= geom.Line3d(pt1.getPos,pt2.getPos)
ang= math.pi/2.0
rot1= xc.Rotation(geom.Rotation3d(axis,-ang))

pt4= points.newPoint(rot1.getTrfPos(geom.Pos3d(0.0, 1.0, 0.0)))

dist34= pt3.dist(pt4.getPos)
#print("dist(3,4)= ",dist34)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (dist34<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
