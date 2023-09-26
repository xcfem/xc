# -*- coding: utf-8 -*-
''' Test the getDict/setFromDict methods.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

# Test problem.
fePrbA= xc.FEProblem()
prepA=  fePrbA.getPreprocessor

# Create points
points= prepA.getMultiBlockTopology.getPoints
ptA= points.newPoint(geom.Pos3d(R,0.0,0.0))
ptB= points.newPoint(geom.Pos3d((R*cos45),(R*sin45),0.0))
ptC= points.newPoint(geom.Pos3d(0.0,R,0.0))

# Create arc
lines= prepA.getMultiBlockTopology.getLines
l= lines.newCircleArc(ptA.tag,ptB.tag,ptC.tag)
angc= l.getAngle()
length= l.getLength()

# Get dictionary.
mbtDict= prepA.getMultiBlockTopology.getDict()

fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

mbt= prepB.getMultiBlockTopology
# Populate from the previous dictionary.
mbt.setFromDict(mbtDict)

# Get the corresponding points.
pointsB=  prepB.getMultiBlockTopology.getPoints
ptA2= pointsB.get(ptA.tag)
ptB2= pointsB.get(ptB.tag)
ptC2= pointsB.get(ptC.tag)

# Check that the positions are the same.
d1= ptA2.dist(ptA.getPos)
d2= ptB2.dist(ptB.getPos)
d3= ptC2.dist(ptC.getPos)

# Get the corresponding arc.
lines2=  prepB.getMultiBlockTopology.getLines
l2= lines2.get(l.tag)
# Check that they have the same length.
d4= l2.getLength()-l.getLength()
# and the same angle.
d5= l2.getAngle()-l.getAngle()

err= math.sqrt(d1**2+d2**2+d3**2+d4**2+d5**2)


'''
# print(mbtDict)
print(d1)
print(d2)
print(d3)
print(err)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

