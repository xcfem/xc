# -*- coding: utf-8 -*-
''' Verifications of the offset method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

pol1=geom.Polyline2d()

pol1.appendVertex(geom.Pos2d(0,0))
pol1.appendVertex(geom.Pos2d(1,0))
pol1.appendVertex(geom.Pos2d(1,1))
pol1.appendVertex(geom.Pos2d(0,1))

longPol1=pol1.getLength()
pol2=pol1.offset(-0.25)
longPol2=pol2.getLength()

ratio1=(longPol1-3)/3.
ratio2=(longPol2-2)/2.

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-10:
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

