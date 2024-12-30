# -*- coding: utf-8 -*-
''' Trivial check of 3D blocks.'''

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Material definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d", 1e6,0.25, 0.0)

#
#     5 +---------+ 8     0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
#      /|        /|       1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
#     / |       / |       2: Front face; vertices 2,3,7,6. (1,2,6,5)
#  6 +---------+7 |       3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
#    |  |      |  |       4: Back face; vertices 1,5,8,4. (0,4,7,3)
#    |1 +------|--+ 4     5: Top face; vertices 5,6,7,8. (4,5,6,7)
#    | /       | /
#    |/        |/
#  2 +---------+ 3
#

# Define k-points.
pt1= modelSpace.newKPoint(0,0,0)
pt2= modelSpace.newKPoint(1,0,0)
pt3= modelSpace.newKPoint(1,1,0)
pt4= modelSpace.newKPoint(0,1,0)

pt5= modelSpace.newKPoint(0,0,1)
pt6= modelSpace.newKPoint(1,0,1)
pt7= modelSpace.newKPoint(1,1,1)
pt8= modelSpace.newKPoint(0,1,1)

# Define block.
bodies= preprocessor.getMultiBlockTopology.getBodies
b1= bodies.newBlockPts(pt1.tag, pt2.tag, pt3.tag, pt4.tag, pt5.tag, pt6.tag, pt7.tag, pt8.tag)
b1.nDivI= 1
b1.nDivJ= 1
b1.nDivK= 2

# Get the vertices with the block internal order.
vertices= b1.getVertices()
internalOrderOfVertices= [pt1, pt4, pt3, pt2, pt5, pt6, pt7, pt8]
vDiff= 0
for v1, v2 in zip(vertices, internalOrderOfVertices):
    vDiff+= abs(v1.tag-v2.tag)

# Compute block volume
volume= b1.getVolume()
ratio1= abs(volume-1)

# Mesh definition.
## Seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast3d.name
brick= seedElemHandler.newElement("Brick")

b1.genMesh(xc.meshDir.I)

refCentroids= [geom.Pos3d(0.5, 0.5, 0.25), geom.Pos3d(0.5, 0.5, 0.75)]
err= 0.0
for e, refCen in zip(b1.elements, refCentroids):
    centroid= e.getPosCentroid(True)
    err+= centroid.dist2(refCen)

err= math.sqrt(err)

# Check fill downwards.
testSet= modelSpace.defSet(setName= 'testSet', bodies= [b1])
testSet.fillDownwards()
numBodies= len(testSet.bodies)
numSurfaces= len(testSet.surfaces)
numLines= len(testSet.lines)
numPoints= len(testSet.points)
numElements= len(testSet.elements)
numNodes= len(testSet.nodes)
fillDownwardsOK= (numBodies==1) and (numSurfaces==6) and (numLines==12) and (numPoints==8) and (numElements==2) and (numNodes==12)

'''
print(numBodies)
print(numSurfaces)
print(numLines)
print(numPoints)
print(numElements)
print(numNodes)
print(fillDownwardsOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((err<=1e-8) and (ratio1<=1e-8) and (vDiff==0) and fillDownwardsOK):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
    
# #Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayLoads()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX')
# # oh.displayDispRot(itemToDisp='uY')
# # oh.displayDispRot(itemToDisp='uZ')
