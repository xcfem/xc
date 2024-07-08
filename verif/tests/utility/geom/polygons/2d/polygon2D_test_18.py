# -*- coding: utf-8 -*-
''' Compute offset polygon test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

pol1=geom.Polygon2d([geom.Pos2d(0.6,-0.4), geom.Pos2d(0.6,0.4), geom.Pos2d(-0.6,0.4), geom.Pos2d(-0.6, -0.4)])

perimPol1= pol1.getPerimeter()
perimPol1Ref= 4*(0.6+0.4)

bufferDistance= 0.63
pol2=pol1.getBuffer(bufferDistance)
perimPol2= pol2.getPerimeter()
perimPol2Ref= perimPol1Ref+2*math.pi*bufferDistance

ratio1= abs(perimPol1-perimPol1Ref)/perimPol1Ref
ratio2= abs(perimPol2-perimPol2Ref)/perimPol2Ref

'''
print(perimPol1, perimPol1Ref, ratio1)
print(perimPol2, perimPol2Ref, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-4 and abs(ratio2)<1e-3:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# import matplotlib.pyplot as plt
# def getXYVertices(plg):
#     x= list()
#     y= list()
#     vertices= plg.getVertices()
#     for p in vertices:
#         x.append(p.x)
#         y.append(p.y)
#     x.append(vertices[0].x)
#     y.append(vertices[0].y)
#     return x,y
    
# fig = plt.figure()
# ax = fig.add_subplot(111)
# x,y= getXYVertices(pol1)
# ax.plot(x,y,'tab:blue')
# x,y= getXYVertices(pol2)
# ax.plot(x,y,'tab:red')
# plt.show()
