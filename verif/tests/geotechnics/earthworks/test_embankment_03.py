# -*- coding: utf-8 -*-
''' Test Embankment layer class.'''

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
#          p6 +------+ p7
#            /        \         <- Ballast.
#     p2 +--+----------+--+ p3
#       /  p5          p8  \
#      /                    \   <- Fill.
#     /                      \
# p1 +                        + p4
# 

p1= geom.Pos3d(0, 0, 0)
p2= geom.Pos3d(1, 0, 1)
p3= geom.Pos3d(5, 0, 1)
p4= geom.Pos3d(6, 0, 0)
p5= geom.Pos3d(1.5, 0, 1)
p6= geom.Pos3d(1.6, 0, 1.5)
p7= geom.Pos3d(4.4, 0, 1.5)
p8= geom.Pos3d(4.5, 0, 1)

# Vertices lists.
fillVertices= [p1, p2, p3, p4]
ballastVertices= [p5, p6, p7, p8]

# Define soils.
## Fill
fillGamma= 20e3 # N/m3
fillRho= fillGamma/g
fill= fs.FrictionalSoil(phi= math.radians(30), rho= fillRho)
fillLoadSpreadingRatio= 2/1 # 2 vertical : 1 horizontal.
## Ballast.
ballastGamma= 22e3 # N/m3
ballastRho= ballastGamma/g
ballast= fs.FrictionalSoil(phi= math.radians(35), rho= ballastRho)
ballastLoadSpreadingRatio= 1.5/1 # 2 vertical : 1 horizontal.

# Construct embankment.
embankment= embankment.EmbankmentCrossSection()
embankment.appendLayer(soil= ballast, loadSpreadingRatio= ballastLoadSpreadingRatio, layerSurfacePoints= ballastVertices)
embankment.appendLayer(soil= fill, loadSpreadingRatio= fillLoadSpreadingRatio, layerSurfacePoints= fillVertices)

# Test points
testPoints= [geom.Pos3d(-1.0, 0, 0), geom.Pos3d(0.5, 0, 0), geom.Pos3d(1, 0, 0), geom.Pos3d(2, 0, 1), geom.Pos3d(3, 0, 0), geom.Pos3d(4,0,0), geom.Pos3d(5,0,0), geom.Pos3d(6,0,0)]

# Compute thicknesses on each test point.
thicknessValues= list()
weightPressures= list()
for p in testPoints:
    thicknesses= embankment.getLayerThicknesses(point= p)
    thicknessValues.append(thicknesses)
    weightPressure= embankment.getWeightVerticalStresses(point= p)
    weightPressures.append(weightPressure)

# Check results.
## Check thickness results.
thicknessRefValues= [[0.0, 0.0], [0.0, 0.5], [0.0, 1.0], [0.5, 0.0], [0.5, 1.0], [0.5, 1.0], [0.0, 1.0], [0.0, 0.0]]
errTh= 0.0
for vRow, refRow in zip(thicknessValues, thicknessRefValues):
    for v, vRef in zip(vRow, refRow):
        errTh+= (v-vRef)**2
errTh= math.sqrt(errTh)

## Check weight results.
weightRefValues= [0.0, 0.5*fill.gamma(), 1.0*fill.gamma(), 0.5*ballast.gamma(), 0.5*ballast.gamma()+1.0*fill.gamma(), 0.5*ballast.gamma()+1.0*fill.gamma(), 1.0*fill.gamma(), 0.0]
errW= 0.0
for w, wRef in zip(weightPressures, weightRefValues):
    errW+= (w-wRef)**2
errW= math.sqrt(errW)

'''
print('thicknessValues:', thicknessValues)
print('errTh= ', errTh)
print('weightPressures:', weightPressures)
print('errW= ', errW)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (errTh<1e-12) and (errW<1e-12):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
