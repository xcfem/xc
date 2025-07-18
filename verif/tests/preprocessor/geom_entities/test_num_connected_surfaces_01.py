# -*- coding: utf-8 -*-
''' Check lines numConnectedSurfaces propery.'''

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

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

p0= modelSpace.newKPoint(0, 0, 0)
p1= modelSpace.newKPoint(1, 0, 0)
p2= modelSpace.newKPoint(0, 1, 0)
p3= modelSpace.newKPoint(0, -1, 0)
p10= modelSpace.newKPoint(0, 0, 1)
p11= modelSpace.newKPoint(1, 0, 1)
p12= modelSpace.newKPoint(0, 1, 1)
p13= modelSpace.newKPoint(0, -1, 1)

sA= modelSpace.newQuadSurface(p0, p1, p11, p10)
sB= modelSpace.newQuadSurface(p0, p2, p12, p10)
sC= modelSpace.newQuadSurface(p0, p3, p13, p10)

xcTotalSet= modelSpace.getTotalSet()

testOK= False
for l in xcTotalSet.lines:
    iVector= l.getIVector
    if(abs(iVector.z)>0.9): # vertical line.
        ns= l.numConnectedSurfaces
        pt= l.points[0].getPos
        center= (math.sqrt(pt.x**2+pt.y**2)<1e-3)
        if(center):
            testOK= (ns==3)
            
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
# oh.displayBlocks(displayLocalAxes= True)


