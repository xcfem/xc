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

NumDivI= 3
NumDivJ= 2
CooMax= 10

# Test problem.
fePrbA= xc.FEProblem()
prepA=  fePrbA.getPreprocessor

# Create points
points= prepA.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(CooMax,CooMax,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,CooMax,0.0))

# Create quadrilateral surface.
surfaces= prepA.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= NumDivI
s.nDivJ= NumDivJ

# Test set definition
testSet1= prepA.getSets.defSet("testSet1")
testSet1.color= xc.Vector([0.1,0.2,0.3])

testSet1.surfaces.append(s)

# Get dictionary.
prepDict= prepA.getDict()

fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

# Populate from the previous dictionary.
prepB.setFromDict(prepDict)

testSet1B= prepA.getSets.getSet(testSet1.name)
testSet1B.fillDownwards()
numPoints= len(testSet1B.points)
numLines= len(testSet1B.lines)
numFaces= len(testSet1B.surfaces)

err= math.sqrt((numPoints-4)**2+(numLines-4)**2+(numFaces-1)**2)

'''
print(numPoints)
print(err)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

