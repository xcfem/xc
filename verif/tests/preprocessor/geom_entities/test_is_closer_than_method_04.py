# -*- coding: utf-8 -*-
''' Trivial check of isCloserThan method.'''

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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem geometry
## Big quads.
origin= geom.Pos3d(0,0,0)
iVector= geom.Vector3d(1,0,0)
jVector= geom.Vector3d(0,1,0)
bottomPoints= list()
topPoints= list()
for i in range(0,5):
    v= i*iVector
    bp= origin+v
    bottomPoints.append(modelSpace.newKPoint(bp.x, bp.y, bp.z))
    tp= origin+jVector+v
    topPoints.append(modelSpace.newKPoint(tp.x, tp.y, tp.z))

bigQuads= list()
bottomPoint0= bottomPoints[0]
topPoint0= topPoints[0]
for bottomPoint1, topPoint1 in zip(bottomPoints[1:], topPoints[1:]):
    bigQuad= modelSpace.newQuadSurface(bottomPoint0, bottomPoint1, topPoint1, topPoint0)
    bigQuads.append(bigQuad)
    topPoint0= topPoint1
    bottomPoint0= bottomPoint1

### Define set.
bigQuadsSet= modelSpace.defSet('bigQuadsSet')
for bq in bigQuads:
    bigQuadsSet.surfaces.append(bq)
bigQuadsSet.fillDownwards()

## Small quads.
origin= geom.Pos3d(1.2045,-0.4723,0.01)
iVector= geom.Pos3d(1.3813,-0.2955,0.01)-origin
jVector= geom.Vector3d(-iVector.y, iVector.x, 0.0)

bottomPoints= list()
topPoints= list()
for i in range(0,11):
    v= i*iVector
    bp= origin+v
    bottomPoints.append(modelSpace.newKPoint(bp.x, bp.y, bp.z))
    tp= origin+jVector+v
    topPoints.append(modelSpace.newKPoint(tp.x, tp.y, tp.z))

smallQuads= list()
bottomPoint0= bottomPoints[0]
topPoint0= topPoints[0]
for bottomPoint1, topPoint1 in zip(bottomPoints[1:], topPoints[1:]):
    smallQuad= modelSpace.newQuadSurface(bottomPoint0, bottomPoint1, topPoint1, topPoint0)
    smallQuads.append(smallQuad)
    topPoint0= topPoint1
    bottomPoint0= bottomPoint1

### Define set.
smallQuadsSet= modelSpace.defSet('smallQuadsSet')
for bq in smallQuads:
    smallQuadsSet.surfaces.append(bq)
smallQuadsSet.fillDownwards()

### Generate mesh.
#### Material definition
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E= 30e6,nu= 0.3,rho= 0.0, h= 0.25)
modelSpace.setDefaultMaterial(mat)
modelSpace.newSeedElement("ShellMITC4")
smallQuadsSet.genMesh(xc.meshDir.I)
smallQuadsSet.fillDownwards()

# Start checking isCloserThan method.
nodesCloseEnough= list()
for n in smallQuadsSet.nodes:
    if(bigQuadsSet.isCloserThan(n, 0.02)):
        nodesCloseEnough.append(n)
testOK= (len(nodesCloseEnough)==115)

for n in nodesCloseEnough:
    pos= n.getInitialPos3d
    ptOK= (pos.y>=0.0) and (pos.y<=1.0) and (pos.x>=1.3232) and (pos.x<=2.6768)
    testOK= ptOK and testOK

# Define set.
nodesCloseEnoughSet= modelSpace.defSet(setName= 'nodesCloseEnoughSet', nodes= nodesCloseEnough)
    
'''
print(len(nodesCloseEnough))
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayBlocks(setToDisplay= bigQuadsSet)
# # oh.displayBlocks(setToDisplay= smallQuadsSet)
# # oh.displayBlocks()
# # oh.displayFEMesh(setsToDisplay= [nodesCloseEnoughSet])


