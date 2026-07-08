# -*- coding: utf-8 -*-
''' Verifications of the offset method when the polyline has one vertex
    between collinear segments.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

pol1= geom.Polyline2d([geom.Pos2d(0,0), geom.Pos2d(1,0), geom.Pos2d(2,0), geom.Pos2d(3,0)])
pol2= pol1.offset(-0.25)

# Vertex between collinear segments.
v= pol2[1]
ratio1= math.sqrt((v.x-1)**2+(v.y-0.25)**2)
ratio2= pol1.getLength()-pol2.getLength()

# print(pol2)
# print(v)
# print(ratio1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and math.fabs(ratio2)<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic stuff
# import matplotlib.pyplot as plt

# # polyline
# x1i= list()
# y1i= list()
# for v in pol1.getVertexList():
#     x1i.append(v.x)
#     y1i.append(v.y)
# x2i= list()
# y2i= list()
# for v in pol2.getVertexList():
#     x2i.append(v.x)
#     y2i.append(v.y)
    
# plt.figure()
# plt.axis('equal')
# plt.plot(x1i,y1i)
# plt.plot(x2i,y2i)
# plt.show()

