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

CooMax= 10.0

# Test problem.
fePrbA= xc.FEProblem()
prepA=  fePrbA.getPreprocessor

# Create points
points= prepA.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(CooMax,CooMax,CooMax))

# Create line.
lines= prepA.getMultiBlockTopology.getLines
l= lines.newLine(pt1.tag,pt2.tag)

# Get dictionary.
mbtDict= prepA.getMultiBlockTopology.getDict()

# Create new problem.
fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

# Populate from the previous dictionary.
mbt= prepB.getMultiBlockTopology
mbt.setFromDict(mbtDict)

# Get the corresponding points.
pointsB=  prepB.getMultiBlockTopology.getPoints
ptB1= pointsB.get(pt1.tag)
ptB2= pointsB.get(pt2.tag)

# Check that the positions are the same.
d1= ptB1.dist(pt1.getPos)
d2= ptB2.dist(pt2.getPos)

# Get the corresponding line.
linesB=  prepB.getMultiBlockTopology.getLines
lB= linesB.get(l.tag)

# Check that they have the same length.
d3= lB.getLength()-l.getLength()

err= math.sqrt(d1**2+d2**2+d3**3)


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
    

