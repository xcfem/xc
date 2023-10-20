# -*- coding: utf-8 -*-
''' Test that the conciliaNDivs method works correctly when the block topology is readed from a dictionary.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import json
import geom
import xc
import os
import sys

CooMaxX= 2
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density
old_stderr= sys.stderr

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler


# Problem geometry.
#                           p2
#  p3  +--------------------+--------------------+ p5
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      |                    |                    |
#      +--------------------+--------------------+
#     p0                    p1                   p4

## K-points.
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPoint(geom.Pos3d(0,0,0))
p1= pointHandler.newPoint(geom.Pos3d(1,0,0))
p2= pointHandler.newPoint(geom.Pos3d(1,1,0))
p3= pointHandler.newPoint(geom.Pos3d(0,1,0))
p4= pointHandler.newPoint(geom.Pos3d(2,0,0))
p5= pointHandler.newPoint(geom.Pos3d(2,1,0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(p0.tag, p1.tag, p2.tag, p3.tag)
s2= surfaces.newQuadSurfacePts(p1.tag, p4.tag, p5.tag, p2.tag)
    
xcTotalSet= preprocessor.getSets.getSet('total')
pointConnectivity= list()
for p in xcTotalSet.points:
    pointConnectivity.append(p.getNumConnectedEdges)
    
lineConnectivity= list()
for l in xcTotalSet.lines:
    lineConnectivity.append(l.getNumConnectedSurfaces)

# Check result.
err= 0
pointConnectivityTestValues= [2, 3, 3, 2, 2, 2]
for v1, v2 in zip(pointConnectivityTestValues, pointConnectivity):
    err+= (v1-v2)

lineConnectivityTestValues= [1, 2, 1, 1, 1, 1, 1]
for v1, v2 in zip(lineConnectivityTestValues, lineConnectivity):
    err+= (v1-v2)
okBefore= (err==0)
    

# Write model to JSON file.
outputData= preprocessor.getDict()

# Create new problem.
fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

# Populate from the previous dictionary.
prepB.setFromDict(outputData)

xcTotalSetB= prepB.getSets.getSet('total')

pointConnectivityB= list()
for p in xcTotalSetB.points:
    pointConnectivityB.append(p.getNumConnectedEdges)
lineConnectivityB= list()
for l in xcTotalSetB.lines:
    lineConnectivityB.append(l.getNumConnectedSurfaces)

# Check values.
errB= 0
for v1, v2 in zip(pointConnectivityTestValues, pointConnectivityB):
    err+= (v1-v2)
for v1, v2 in zip(lineConnectivityTestValues, lineConnectivityB):
    errB+= (v1-v2)
okAfter= (errB==0)

'''
print(pointConnectivity)
print(lineConnectivity)
print(okBefore)
print(pointConnectivityB)
print(lineConnectivityB)
print(okAfter)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(okBefore and okAfter):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
