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

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
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

pt9= modelSpace.newKPoint(1,2,0)
pt10= modelSpace.newKPoint(0,2,0)

pt11= modelSpace.newKPoint(1,2,1)
pt12= modelSpace.newKPoint(0,2,1)

# Define block.
bodies= preprocessor.getMultiBlockTopology.getBodies
b1= bodies.newBlockPts(pt1.tag, pt2.tag, pt3.tag, pt4.tag, pt5.tag, pt6.tag, pt7.tag, pt8.tag)
b2= bodies.newBlockPts(pt4.tag, pt3.tag, pt9.tag, pt10.tag, pt8.tag, pt7.tag, pt11.tag, pt12.tag)

# Get the vertices with the block internal order.
vertices1= b1.getVertices()
internalOrderOfVertices1= [pt1, pt4, pt3, pt2, pt5, pt6, pt7, pt8]
vDiff= 0
for v1, v2 in zip(vertices1, internalOrderOfVertices1):
    vDiff+= abs(v1.tag-v2.tag)
vertices2= b2.getVertices()
internalOrderOfVertices2= [pt4, pt10, pt9, pt3, pt8, pt7, pt11, pt12]
for v1, v2 in zip(vertices2, internalOrderOfVertices2):
    vDiff+= abs(v1.tag-v2.tag)

vl= b2.getVerbosityLevel() # Get current verbosity level.
b2.setVerbosityLevel(0) # Don't bother with warning messages
                        # that will be solved.
b2.nDivI= 2
b2.nDivJ= 2
b2.nDivK= 2
b2.setVerbosityLevel(vl) # Restore previous verbosity level.

modelSpace.conciliaNDivs()

# Mesh definition.
## Seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast3d.name
brick= seedElemHandler.newElement("Brick")

xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.fillDownwards()
xcTotalSet.genMesh(xc.meshDir.I)

refCentroids= [geom.Pos3d(0.25, 0.125, 0.25), geom.Pos3d(0.25, 0.375, 0.25), geom.Pos3d(0.25, 0.625, 0.25), geom.Pos3d(0.25, 0.875, 0.25), geom.Pos3d(0.75, 0.125, 0.25), geom.Pos3d(0.75, 0.375, 0.25), geom.Pos3d(0.75, 0.625, 0.25), geom.Pos3d(0.75, 0.875, 0.25), geom.Pos3d(0.25, 0.125, 0.75), geom.Pos3d(0.25, 0.375, 0.75), geom.Pos3d(0.25, 0.625, 0.75), geom.Pos3d(0.25, 0.875, 0.75), geom.Pos3d(0.75, 0.125, 0.75), geom.Pos3d(0.75, 0.375, 0.75), geom.Pos3d(0.75, 0.625, 0.75), geom.Pos3d(0.75, 0.875, 0.75), geom.Pos3d(0.25, 1.25, 0.25), geom.Pos3d(0.25, 1.75, 0.25), geom.Pos3d(0.75, 1.25, 0.25), geom.Pos3d(0.75, 1.75, 0.25), geom.Pos3d(0.25, 1.25, 0.75), geom.Pos3d(0.25, 1.75, 0.75), geom.Pos3d(0.75, 1.25, 0.75), geom.Pos3d(0.75, 1.75, 0.75)]

err= 0.0
for e, refCen in zip(b1.elements, refCentroids):
    centroid= e.getPosCentroid(True)
    err+= centroid.dist2(refCen)

err= math.sqrt(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((err<=1e-8) and (vDiff==0)):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
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
