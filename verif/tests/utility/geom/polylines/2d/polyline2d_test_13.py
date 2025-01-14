# -*- coding: utf-8 -*-
''' Verification of the getBufferPolygon method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

import math
pi = math.pi

def points_in_circum(r,n=10):
    coords= [(math.cos(2*pi/n*x)*r,math.sin(2*pi/n*x)*r) for x in range(0,n+1)]
    retval= list()
    for xy in coords[1:]:
        retval.append(geom.Pos2d(xy[0], xy[1]))
    return retval

# Create test polyline.
points= points_in_circum(r= 50, n= 10)
pline= geom.Polyline2d(points)
bufferOffset= 2
plineBuffer= pline.getBufferPolygon(bufferOffset, 8)

# Check results
area= plineBuffer.getArea()
refArea= pline.getLength()*bufferOffset*2+math.pi*bufferOffset**2
ratio= abs(area-refArea)/refArea

# print(area, refArea, ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio<1e-2:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


# # Graphic stuff
# import matplotlib.pyplot as plt

# # polyline
# xi= list()
# yi= list()
# for v in pline.getVertexList():
#     xi.append(v.x)
#     yi.append(v.y)
    
# plt.figure()
# plt.axis('equal')
# plt.plot(xi,yi)

# # polygon
# xs= list()
# ys= list()
# vertices= plineBuffer.getVertexList()
# vertices.append(vertices[0]) # Close the polygon.
# for v in vertices:
#     xs.append(v.x)
#     ys.append(v.y)

# plt.plot(xs,ys)

# plt.show()
