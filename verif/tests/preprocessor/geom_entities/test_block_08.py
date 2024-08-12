# -*- coding: utf-8 -*-
''' Trivial check of 3D blocks.'''

from __future__ import division
from __future__ import print_function

import math
import numpy as np
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
xyzList= list()
coord= [0, 1, 2, 3]
for k in coord:
    layer= list()
    for j in coord:
        row= list()
        for i in coord:
            row.append((i,j,k))
        layer.append(row)
    xyzList.append(layer)


# Define blocks.
bodies= preprocessor.getMultiBlockTopology.getBodies
blocks= list()
for k in [0, 1, 2]:
    for j in [0, 1, 2]:
        for i in [0, 1, 2]:
            p1xyz= xyzList[k][j][i] # (0,0,0)
            p2xyz= xyzList[k][j][i+1] # (1,0,0)
            p3xyz= xyzList[k][j+1][i+1] # (1,1,0)
            p4xyz= xyzList[k][j+1][i] # (0,1,0)
            p5xyz= xyzList[k+1][j][i] # (0,0,1)
            p6xyz= xyzList[k+1][j][i+1] # (1,0,1)
            p7xyz= xyzList[k+1][j+1][i+1] # (1,1,1)
            p8xyz= xyzList[k+1][j+1][i] # (0,1,1)
            
            pt1= modelSpace.newKPoint(p1xyz[0], p1xyz[1], p1xyz[2])
            pt2= modelSpace.newKPoint(p2xyz[0], p2xyz[1], p2xyz[2])
            pt3= modelSpace.newKPoint(p3xyz[0], p3xyz[1], p3xyz[2])
            pt4= modelSpace.newKPoint(p4xyz[0], p4xyz[1], p4xyz[2])

            pt5= modelSpace.newKPoint(p5xyz[0], p5xyz[1], p5xyz[2])
            pt6= modelSpace.newKPoint(p6xyz[0], p6xyz[1], p6xyz[2])
            pt7= modelSpace.newKPoint(p7xyz[0], p7xyz[1], p7xyz[2])
            pt8= modelSpace.newKPoint(p8xyz[0], p8xyz[1], p8xyz[2])
            blk= bodies.newBlockPts(pt1.tag, pt2.tag, pt3.tag, pt4.tag, pt5.tag, pt6.tag, pt7.tag, pt8.tag)
            blocks.append(blk)
            vl= blk.getVerbosityLevel() # Get current verbosity level.
            blk.setVerbosityLevel(0) # Don't bother with warning messages
                        # that will be solved.
            blk.nDivI= 1
            blk.nDivJ= 1
            blk.nDivK= 1
            blk.setVerbosityLevel(vl) # Restore previous verbosity level.
    
# Compute block volumes
volErr= 0.0
for blk in blocks:
    v= blk.getVolume()
    volErr+= (v-1)**2
volErr= math.sqrt(volErr)

# Mesh definition.
## Seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast3d.name
brick= seedElemHandler.newElement("Brick")

xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.genMesh(xc.meshDir.I)

refCentroids= list()
for z in [0.5, 1.5, 2.5]:
    for y in [0.5, 1.5, 2.5]:
        for x in [0.5, 1.5, 2.5]:
            refCentroids.append(geom.Pos3d(x,y,z))

computedCentroids= list()
for blk in blocks:
    for e in blk.elements:
        computedCentroids.append(e.getPosCentroid(True))
# Compare results.        
err= 0.0
for compCen, refCen in zip(computedCentroids, refCentroids):
    err+= compCen.dist2(refCen)

err= math.sqrt(err)

    
# print(volErr)
# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((volErr<1e-8) and (err<1e-8)):
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
