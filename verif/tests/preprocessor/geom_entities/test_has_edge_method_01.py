# -*- coding: utf-8 -*-
''' Use hasEdge method to obtain the peripheral faces of a set.'''

from __future__ import division
from __future__ import print_function

import math
import geom
import xc
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# +-----+-----+-----+-----+
# |     |     |     |     |
# |  P  |  P  |  P  |  P  |
# |     |     |     |     |
# +-----+-----+-----+-----+       P: peripheral surface
# |     |     |     |     |       C: central surface
# |  P  |  C  |  C  |  P  |
# |     |     |     |     |
# +-----+-----+-----+-----+
# |     |     |     |     |
# |  P  |  C  |  C  |  P  |
# |     |     |     |     |
# +-----+-----+-----+-----+
# |     |     |     |     |
# |  P  |  P  |  P  |  P  |
# |     |     |     |     |
# +-----+-----+-----+-----+

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

points= list()
for j in range(0,5):
    point_row= list()
    for i in range(0,5):
        point_row.append(modelSpace.newKPoint(i,j,0))
    points.append(point_row)

quads= list()

p0= points[0][0]
p1= points[0][0]
for i in range(0,4):
    for j in range(0, 4):
        p0= points[i][j]
        p1= points[i+1][j]
        p2= points[i+1][j+1]
        p3= points[i][j+1]
        quads.append(modelSpace.newQuadSurface(p0, p1, p2, p3))

# Define set.
quadsSet= modelSpace.defSet('quadsSet')
for q in quads:
    quadsSet.surfaces.append(q)
quadsSet.fillDownwards()

## Compute peripheral lines.
peripheralLines= list()
for l in quadsSet.lines:
    nSurfaces= l.getNumConnectedSurfaces
    if(nSurfaces<2):
        peripheralLines.append(l)
## Compute peripheral surfaces.
peripheralSurfaces= list()
centralSurfaces= list()
for s in quadsSet.surfaces:
    peripheralSurface= False
    for l in peripheralLines:
        if(s.hasEdge(l)):
            peripheralSurface= True
    if(peripheralSurface):
        peripheralSurfaces.append(s)
    else:
        centralSurfaces.append(s)

# Check results
testOK= (len(peripheralSurfaces)==12)
for ps in peripheralSurfaces:
    centroid= ps.getCentroid()
    if((centroid.x>0.5) or (centroid.x<3.5)):
        psOK= testOK and ((centroid.y==0.5) or (centroid.y==3.5))
    elif((centroid.y>0.5) or (centroid.y<3.5)):
        testOK= testOK and ((centroid.x==0.5) or (centroid.x==3.5))
    if(not testOK):
        break
    
testOK= (len(centralSurfaces)==4)
for ps in centralSurfaces:
    centroid= ps.getCentroid()
    testOK= testOK and ((centroid.x>0.5) and (centroid.x<3.5))
    testOK= testOK and ((centroid.y>0.5) and (centroid.y<3.5))

'''
print(peripheralSurfaces)
print(centralSurfaces)
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

# oh.displayBlocks()
