# -*- coding: utf-8 -*-
''' Test Embankment layer class. Same test as in the file test_embankment_01 
 but with the axes X and Y swapped.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geotechnics.earthworks import embankment
from geotechnics import frictional_soil as fs
from scipy.constants import g

# Embankment points.
#     p2 +--------+ p3
#       /          \
#      /            \
#     /              \
# p1 +                + p4
# 

# Vertex list.
vertices= [geom.Pos3d(0, 0, 0), geom.Pos3d(0, 1, 1), geom.Pos3d(0, 2, 1), geom.Pos3d(0, 3, 0)]

# Define polyline.
plane_pline_3d= geom.PlanePolyline3d(vertices)

# Define soil.
soilGamma= 20e3 # N/m3
soilRho= soilGamma/g
soil= fs.FrictionalSoil(phi= math.radians(30), rho= soilRho)

# Define embankment layer.
embankmentLayer= embankment.EmbankmentLayer(pline= plane_pline_3d, soil= soil)

# Compute depths of the surface vertices (must be zero).
depths0= embankmentLayer.getDepths(pointList= vertices)

# Try some other points
testPointsA= [geom.Pos3d(10.0, 0.5, 1.0), geom.Pos3d(15.0, 1.5, 1.5), geom.Pos3d(-12.0, 2.5, 1)]
depthsA= embankmentLayer.getDepths(pointList= testPointsA)

testPointsB= [geom.Pos3d(10.0, 0.5, -1.0), geom.Pos3d(15.0, 1.5, -0.5), geom.Pos3d(-12.0, 2.5, -1)]
depthsB= embankmentLayer.getDepths(pointList= testPointsB)

values= [depths0, depthsA, depthsB]
refValues= [4*[0.0], 3*[-0.5], 3*[1.5]]
err= 0.0

for vRow, refRow in zip(values, refValues):
    for v, vRef in zip(vRow, refRow):
        err+= (v-vRef)**2
err= math.sqrt(err)

'''
print(depths0)
print(depthsA)
print(depthsB)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

