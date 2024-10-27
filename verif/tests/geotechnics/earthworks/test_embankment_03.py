# -*- coding: utf-8 -*-
''' Test Embankment layer class. Embankment contained in a vertical arbitrary
plane.'''

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
# p1 +                + p4  ---> Y axis
# 

# Vertex list.
vertices= [ geom.Pos3d(-9222.584640377701e-3, 27991.734507999256e-3, -2774.515625000002e-3),
            geom.Pos3d(-9032.247070312502e-3, 28211.185546875e-3, 1293.1621093750005e-3),
            geom.Pos3d(-5515.166992187498e-3, 32207.033203125004e-3, 1613.48046875e-3),
            geom.Pos3d(-2193.839111328123e-3, 35980.48046875e-3, 1310.9999999999995e-3),
            geom.Pos3d(1623.8026123047111e-3, 40548.01171875003e-3, -2819.0000000000005e-3)]

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
testPointsA= [geom.Pos3d(5901.36865234375e-3, 36112.4765625e-3, 0.0),
              geom.Pos3d(-2754.543701171875e-3, 36112.4765625e-3, 0.0),
              geom.Pos3d(-12437.34765625e-3, 31208.30078125e-3, 0.0),
              geom.Pos3d(-8689.416015625e-3, 27909.431640625e-3, 0.0),
              geom.Pos3d(-1746.2445068359375e-3, 31571.00390625e-3, 0.0),
              geom.Pos3d(7165.8857421875e-3, 31553.732421875e-3, 0.0)]
depthsA= embankmentLayer.getDepths(pointList= testPointsA)
#refDepthsA= [-2.4372614817550327, 1.327096104165939, 1.295244642361503, 1.2443776804356421, 1.4938632000600398, -0.6215704066092139]
refDepthsA= [-2.52345197085698, 1.3337299788904373, 1.3558628720885595, 1.2441318084068953, 1.4496270701506797, -0.7631503206593947]
values= [depths0, depthsA]
refValues= [5*[0.0], refDepthsA]
err= 0.0

for vRow, refRow in zip(values, refValues):
    for v, vRef in zip(vRow, refRow):
        err+= (v-vRef)**2
err= math.sqrt(err)

'''
print('depths0 ='+str(depths0))
print('depths0 ='+str(depthsA))
print('refDepthsA ='+str(refDepthsA))
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-7):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

