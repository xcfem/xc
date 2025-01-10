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

# Create test polyline.
points= [geom.Pos2d(11.0738, 20.218), geom.Pos2d(-22.9508, 15.2622)]
pol1= geom.Polyline2d(points)

# Compute offset.
offsetDist= 0.55
pol1Offset= pol1.offset(offsetDist)

# Check results.
testVector= offsetDist*pol1.getJVectorAtLength(0.5*pol1.getLength())
err= 0
for pA, pB in zip(pol1.getVertexList(), pol1Offset.getVertexList()):
    v= pB-pA
    err+= (v.x-testVector.x)**2+(v.y-testVector.y)**2
err= math.sqrt(err)

'''
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
# xi= list()
# yi= list()
# for v in pol1.getVertexList():
#     xi.append(v.x)
#     yi.append(v.y)
    
# plt.figure()
# plt.axis('equal')
# plt.plot(xi,yi)

# # positive offset
# xs1= list()
# ys1= list()
# for v in pol1Offset.getVertexList():
#     xs1.append(v.x)
#     ys1.append(v.y)


# plt.plot(xs1,ys1)

# plt.show()
