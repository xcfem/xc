# -*- coding: utf-8 -*-

from __future__ import division
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

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
ptA= points.newPntFromPos3d(geom.Pos3d(R,0.0,0.0))
ptB= points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
ptC= points.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))

lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newCircleArc(ptA.tag,ptB.tag,ptC.tag)
angc= l.getAngle()
length= l.getLength()


angteor= math.pi/2.0
ratio1= (angteor/angc)
lteor= 2*math.pi*R/4.0
ratio2= (lteor/length)

# print("Included angle: ",math.degrees(angc))
# print("theoretical included angle: ",math.degrees(angteor))
# print("Arc length: ",length)
# print("Theorethical length: ",lteor)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) and (abs(ratio2-1.0)<1e-5):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

