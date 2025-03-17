# -*- coding: utf-8 -*-
''' Check connectedEdges and getConnectedEdges methods.'''


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Create a ladder like model.
points= [list(), list()]
for i in range(0, 10):
    points[0].append(modelSpace.newKPoint(0, i, 0))
    points[1].append(modelSpace.newKPoint(2, i, 0))

beams= [list(), list()]
for i in [0, 1]:
    p0= points[i][0]
    for p1 in points[i][1:]:
        beams[i].append(modelSpace.newLine(p0, p1))
        p0= p1

steps= list()
for p0, p1 in zip(points[0][1:-1], points[1][1:-1]):
    steps.append(modelSpace.newLine(p0, p1))

# Create sets.
leftBeamSet= modelSpace.defSet('leftBeamSet')
for l in beams[0]:
    leftBeamSet.lines.append(l)
leftBeamSet.fillDownwards()

rightBeamSet= modelSpace.defSet('rightBeamSet')
for l in beams[1]:
    rightBeamSet.lines.append(l)
rightBeamSet.fillDownwards()
stepsSet= modelSpace.defSet('stepsSet')
for l in steps:
    stepsSet.lines.append(l)
stepsSet.fillDownwards()

testOK= True
nPoints= len(leftBeamSet.points)
numConnectedEdgesRef= [1]+(nPoints-2)*[3]+[1]
numStepsConnectedEdgesRef= [0]+(nPoints-2)*[1]+[0]
numSelfConnectedEdgesRef= [1]+(nPoints-2)*[2]+[1]
for p, nCRef, nSCRef, nSfCRef in zip(leftBeamSet.points, numConnectedEdgesRef, numStepsConnectedEdgesRef, numSelfConnectedEdgesRef):
    nC= p.numConnectedEdges
    nSC= p.getNumConnectedEdges(stepsSet)
    nSfC= p.getNumConnectedEdges(leftBeamSet)
    testOK= testOK and (nC==nCRef) and (nSC== nSCRef) and (nSfC==nSfCRef)
for p, nCRef, nSCRef, nSfCRef in zip(rightBeamSet.points, numConnectedEdgesRef, numStepsConnectedEdgesRef, numSelfConnectedEdgesRef):
    nC= p.numConnectedEdges
    nSC= p.getNumConnectedEdges(stepsSet)
    nSfC= p.getNumConnectedEdges(rightBeamSet)
    testOK= testOK and (nC==nCRef) and (nSC== nSCRef) and (nSfC==nSfCRef)
    
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
