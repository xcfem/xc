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
nodes= preprocessor.getNodeHandler

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)

length= l1.getLength()
l1.splitAtNaturalCoordinate(0.0)
length1= l1.getLength()
l2= lines.get(1)
length2= l2.getLength()


ratio1= abs(length1-length/2)
ratio2= abs(length2-length/2)

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

