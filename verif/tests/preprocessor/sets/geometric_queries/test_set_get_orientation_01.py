# -*- coding: utf-8 -*-
''' Check getOrientation method applied to k-Points.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry.
org= geom.Pos3d(0,0,0)
v= 0.5*geom.Vector3d(-1,1,0).normalized()
pA= org+v
pB= org-v
dirVector= geom.Vector3d(1,1,1)

# Create k-points.
aPoints= list()
bPoints= list()
for i in range(0,11):
    newPoint= pA+i*dirVector
    aPoints.append(modelSpace.newKPoint(newPoint.x, newPoint.y, newPoint.z))
    newPoint= pB+i*dirVector
    bPoints.append(modelSpace.newKPoint(newPoint.x, newPoint.y, newPoint.z))

# Create surfaces.
p0a= aPoints[0]
p0b= bPoints[0]
for p1a, p1b in zip(aPoints[1:], bPoints[1:]):
    s= modelSpace.newQuadSurface(p0b, p1b, p1a, p0a)
    p0a= p1a
    p0b= p1b

xcTotalSet= modelSpace.getTotalSet()

orientation= xcTotalSet.getEntities.getOrientation()

axis1= orientation.getAxis1VDir()
axis2= orientation.getAxis2VDir()
axis3= orientation.getAxis3VDir()

# Check results
sqrt_3_3= math.sqrt(3)/3
axis1Ref= geom.Vector3d(sqrt_3_3, sqrt_3_3, sqrt_3_3)
err= (axis1-axis1Ref).getModulus2()
sqrt_2_2= math.sqrt(2)/2
axis2Ref= geom.Vector3d(-sqrt_2_2, sqrt_2_2, 0)
err+= (axis2-axis2Ref).getModulus2()
axis3Ref= axis1Ref.cross(axis2Ref)
err+= (axis3-axis3Ref).getModulus2()

err= math.sqrt(err)

'''
print(axis1, axis1Ref)
print(axis2, axis2Ref)
print(axis3, axis3Ref)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
