# -*- coding: utf-8 -*-
''' Verification of the getBufferPolygon method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

# Create test polyline.
points= [geom.Pos2d(0.202875, 0.688221), geom.Pos2d(4.09401, -0.458814), geom.Pos2d(8.03093, -1.32846), geom.Pos2d(12.0186, -1.92359), geom.Pos2d(16.0379, -2.24133), geom.Pos2d(20.0695, -2.28017), geom.Pos2d(24.0942, -2.03992), geom.Pos2d(28.0925, -1.52174), geom.Pos2d(32.0455, -0.7281), geom.Pos2d(35.934, 0.337187), geom.Pos2d(39.7526, 1.67359), geom.Pos2d(45.5433, 3.90163), geom.Pos2d(51.5429, 5.63965), geom.Pos2d(57.6687, 6.86005), geom.Pos2d(63.8762, 7.55396), geom.Pos2d(70.1203, 7.71633), geom.Pos2d(76.3555, 7.34597), geom.Pos2d(82.5364, 6.44559), geom.Pos2d(88.6182, 5.02173), geom.Pos2d(94.5564, 3.08474), geom.Pos2d(100.28, 0.660685)]
pol1=geom.Polyline2d(points)
lastPointBefore= pol1.getToPoint()
epsilon= pol1.getLength()/1e3 # compute a reasonable epsilon.
pol1.simplify(epsilon) # simplify the polyline.
lastPointAfter= pol1.getToPoint()
err= lastPointBefore.dist(lastPointAfter)

'''
print(lastPointBefore)
print(lastPointAfter)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


# # Graphic stuff
# import matplotlib.pyplot as plt

# # polyline
# pol1Coo= list()
# for p in points:
#     pol1Coo.append([p.x, p.y])

# xs, ys = zip(*pol1Coo) #create lists of x and y values

# plt.figure()
# plt.plot(xs,ys)

# simplifiedCoo= list()
# for p in pol1.getVertexList():
#     simplifiedCoo.append([p.x, p.y])
# xs, ys = zip(*simplifiedCoo) #create lists of x and y values
# plt.plot(xs, ys)
# plt.show()
