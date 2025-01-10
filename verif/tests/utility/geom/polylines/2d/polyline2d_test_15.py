# -*- coding: utf-8 -*-
''' Verification of the getBufferPolygon method. Test with flat endings.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import numpy as np

def get_points():
    '''Compute the x and y coordinates for points on a sine curve.'''
    xi= np.arange(0, 3 * np.pi, 0.2)
    yi= np.sin(xi)
    retval= list()
    for x, y in zip(xi, yi):
        retval.append(geom.Pos2d(x,y))
    return retval

# Create test polyline.
points= get_points()
pline= geom.Polyline2d(points)
bufferOffset= 0.55
plineBuffer= pline.getBufferPolygon(bufferOffset, 0)

# Check results
area= plineBuffer.getArea()
refArea= pline.getLength()*bufferOffset*2
ratio= abs(area-refArea)/refArea

# print(area, refArea, ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio<2e-2:
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
