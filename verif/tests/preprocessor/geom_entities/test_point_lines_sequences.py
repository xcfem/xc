# -*- coding: utf-8 -*-
from __future__ import print_function

import geom
import xc
from model.geometry import geom_utils as gu

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define some points.
pointHandler= preprocessor.getMultiBlockTopology.getPoints
xi= [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]

pointList= list()
for x in xi:
    pointList.append(pointHandler.newPoint(geom.Pos3d(float(x),0.0,0.0)))

# Define some lines.
lineHandler= preprocessor.getMultiBlockTopology.getLines
lineList= list()
firstPoint= pointList[0]
for p in pointList[1:]:
    lineList.append(lineHandler.newLine(firstPoint.tag, p.tag))
    firstPoint= p

## Check get_line_sequence_from_point_sequence.
lineSequence= gu.get_line_sequence_from_point_sequence(pointList)

err= len(lineList)-len(lineSequence)
for lA, lB in zip(lineList, lineSequence):
    err+= lA.tag-lB.tag

## Check get_point_sequence_from_line_sequence
pointSequence= gu.get_point_sequence_from_line_sequence(lineSequence)

err+= len(pointList)-len(pointSequence)

for pA, pB in zip(pointList, pointSequence):
    err+= pA.tag-pB.tag

## Check get_pos3d_sequence_from_line_sequence
pos3dSequence= gu.get_pos3d_sequence_from_line_sequence(lineSequence)

for x, p in zip(xi, pos3dSequence):
    err+= int((x-p.x)**2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err==0:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


