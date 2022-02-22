# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 25
CooMax= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPoint(2,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= NumDiv

length= l1.getLength()
l1.splitAtLambda(0.5)

length1= l1.getLength()
l2= lines.get(2)
length2= l2.getLength()


ratio1= abs(length1-length/2.0)
ratio2= abs(length2-length/2.0)

''' 
print("length: ",length)
print("length1: ",length1)
print("length2: ",length2)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1 < 1e-15) & (ratio2 < 1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


