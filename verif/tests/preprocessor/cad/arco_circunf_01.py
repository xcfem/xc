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

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
points.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))

lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l= lines.newCircleArc(1,2,3)
angc= l.getAngle()
long= l.getLong()

        # \arco_circunf[1]
        #   {
        #     \p1{1} \p2{3} \p3{2}
        #     angc= getAngComp
        #     long= long
        #   }


angteor= math.pi/2.0
ratio1= (angteor/angc)
lteor= 2*math.pi*R/4.0
ratio2= (lteor/long)

# print "Included angle: ",math.degrees(angc)
# print "theoretical included angle: ",math.degrees(angteor)
# print "Arc length: ",long
# print "Theorethical length: ",lteor

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) and (abs(ratio2-1.0)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

