# -*- coding: utf-8 -*-
'''Test the getSymmetric method.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

p2= geom.Pos2d(2.825,-0.6775)
p3= geom.Pos2d(2.825, 0.6775)
p4= geom.Pos2d(2.2275, 1.275)
p5= geom.Pos2d(2.15, 1.225)
xAxis= geom.Line2d(geom.Pos2d(0,0), geom.Vector2d(1, 0))
p0= xAxis.getSymmetric(p5)
p1= xAxis.getSymmetric(p4)

points= [p0, p1, p2, p3, p4, p5]
yAxis= geom.Line2d(geom.Pos2d(0,0), geom.Vector2d(0, 1))
tmp= list()
for p in list(reversed(points)):
    sp= yAxis.getSymmetric(p)
    tmp.append(sp)

points+= tmp
points.append(points[0]) # Close the polygon.

# Check results.
xi= list()
yi= list()
for p in points:
    xi.append(p.x)
    yi.append(p.y)
x_ref= [2.15, 2.2275, 2.825, 2.825, 2.2275, 2.15, -2.15, -2.2275, -2.825, -2.825, -2.2275, -2.15, 2.15]
y_ref= [-1.225, -1.275, -0.6775, 0.6775, 1.275, 1.225, 1.225, 1.275, 0.6775, -0.6775, -1.275, -1.225, -1.225]
err= 0.0
for i, (x, y) in enumerate(zip(xi, yi)):
    err+= (x-x_ref[i])**2
    err+= (y-y_ref[i])**2
    
err= math.sqrt(err)

'''
print(xi)
print(yi)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic stuff.
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
# fig = plt.figure()
# ax = plt.axes()

# ax.plot(xi, yi)

# # Show graph
# plt.show()
