# -*- coding: utf-8 -*-
'''Test Polygon2d duplicated vertices removal.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom

# Define polygons.
## Polygon with "duplicated" end points.
pointsA= [geom.Pos2d(-6.55729e-06,-0.444992), geom.Pos2d(0.00088,-0.75), geom.Pos2d(0.192791,-0.75), geom.Pos2d(0.2165,-0.6615), geom.Pos2d(0.1875,-0.55324), geom.Pos2d(0.10825,-0.474), geom.Pos2d(0,-0.44499)]
plgA= geom.Polygon2d(pointsA)

## Square.
pointsB= [geom.Pos2d(-1,-1), geom.Pos2d(1,-1), geom.Pos2d(1,1), geom.Pos2d(-1, 1)]
plgB= geom.Polygon2d(pointsB)

## Polygon with "duplicated" intermediate vertex.
pointsC= [geom.Pos2d(-0.08268,0.467147), geom.Pos2d(0,0.44499), geom.Pos2d(1e-05,0.44499), geom.Pos2d(0.00027,0.75), geom.Pos2d(-0.08268,0.75)]
plgC= geom.Polygon2d(pointsC)

# Remove duplicated vertices.
cleanedA= geom.remove_duplicated_vertices(plgA, -1.0)
cleanedB= geom.remove_duplicated_vertices(plgB, -1.0)
cleanedC= geom.remove_duplicated_vertices(plgC, -1.0)

# In polygon A the last vertex must be cleaned aut, so it must have one less
# vertex.
numVerticesA= plgA.getNumVertices()
numVerticesCleanedA= cleanedA.getNumVertices()
testOK= ((numVerticesA-numVerticesCleanedA)==1)

# The polygon B must not change.
numVerticesB= plgB.getNumVertices()
numVerticesCleanedB= cleanedB.getNumVertices()
testOK= testOK and (numVerticesB==numVerticesCleanedB)

# In polygon C the intermediate vertex must be cleaned aut, so it must have
# one less vertex.
numVerticesC= plgC.getNumVertices()
numVerticesCleanedC= cleanedC.getNumVertices()
testOK= testOK and  ((numVerticesC-numVerticesCleanedC)==1)

'''
print('plgA= ', plgA, plgA.getNumVertices())
print('cleanedA= ', cleanedA, numVerticesCleanedA)
print('plgB= ', plgB, plgB.getNumVertices())
print('cleanedB= ', cleanedB, numVerticesCleanedB)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
