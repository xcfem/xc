# -*- coding: utf-8 -*-
''' Test Quadrilateral3D class.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

#    
#  p4  +--------------------+ p3
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      |                    |
#      +--------------------+
#     p1                     p2
#

# Create the quadrilateral surface
sideLength= 10
p1= geom.Pos3d(0,0,0)
p2= geom.Pos3d(0,sideLength,0)
p3= geom.Pos3d(0,sideLength,sideLength)
p4= geom.Pos3d(0,0,sideLength)
quad= geom.Quadrilateral3d(p1, p2, p3, p4)

# Compute its area
area= quad.getArea()
refArea= sideLength**2

# Compute the values of the shape functions at vertices.
shapeFunctionsValues= list()
for p in [p1, p2, p3, p4]:
    niValues= quad.Ni(p)
    shapeFunctionsValues.append(niValues)
refShapeFunctionsValues= [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]

# Compute the values of the shape functions at the quadrilateral midpoint.
midpointNiValues= quad.Ni(quad.midpoint)
refMidpointNiValues= 4*[1/4.0]

# Check the results.
ratio1= abs(area-refArea)/refArea
err= 0
for values,refValues in zip(shapeFunctionsValues, refShapeFunctionsValues):
    for ni, refNi in zip(values, refValues):
        err+= (ni-refNi)**2
        
for ni, refNi in zip(midpointNiValues, refMidpointNiValues):
    err+= (ni-refNi)**2

err= math.sqrt(err)

'''
print(ratio1)
print(shapeFunctionsValues)
print(midpointNiValues)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
