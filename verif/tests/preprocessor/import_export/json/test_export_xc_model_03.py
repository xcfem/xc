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

# Get dictionary.
mbtDict= prepA.getMultiBlockTopology.getDict()

fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

mbt= prepB.getMultiBlockTopology
# Populate from the previous dictionary.
mbt.setFromDict(mbtDict)

# Get the corresponding points.
pointsB=  prepB.getMultiBlockTopology.getPoints
pt1B= pointsB.get(pt1.tag)
pt2B= pointsB.get(pt2.tag)
pt3B= pointsB.get(pt3.tag)
pt4B= pointsB.get(pt4.tag)

# Check that the positions are the same.
d1= pt1.dist(pt1B.getPos)
d2= pt2.dist(pt2B.getPos)
d3= pt3.dist(pt3B.getPos)
d4= pt4.dist(pt4B.getPos)

# Get the corresponding arc.
faces2=  prepB.getMultiBlockTopology.getSurfaces
s2= faces2.get(s.tag)
# Check that they have the same area.
d4= s2.getArea()-s.getArea()
# and the same number of divisions.
d5= s2.nDivI-s.nDivI
d6= s2.nDivJ-s.nDivJ

err= math.sqrt(d1**2+d2**2+d3**2+d4**2+d5**2+d6**2)


'''
# print(mbtDict)
print(d1)
print(d2)
print(d3)
print(s2.nDivI)
print(err)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

