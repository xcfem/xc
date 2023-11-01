# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

points= [geom.Pos2d(7489.38, 5463.23), geom.Pos2d(7391.43, 5436.11), geom.Pos2d(7384.67, 5460.99), geom.Pos2d(7525.98, 5501.24), geom.Pos2d(7533.25, 5476.79)]

    
plg= geom.Polygon2d(points)
outPlg= plg.offset(-5)
inPlg= plg.offset(+5)

# Check results.
inLength= inPlg.getLength()
inLengthRef= 306.2164268197953
inArea= inPlg.getArea()
inAreaRef= 2215.5534668420264
outLength= outPlg.getLength()
outLengthRef= 385.627209626014
outArea= outPlg.getArea()
outAreaRef= 5673.288182598288
err= math.sqrt((inLengthRef-inLength)**2+(outLengthRef-outLength)**2+(inAreaRef-inArea)**2+(outAreaRef-outArea)**2)

'''
print(inLength)
print(outLength)
print(inArea)
print(outArea)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-6:
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
# x,y= getXYVertices(plg)
# ax.plot(x,y,'tab:gray')
# x,y= getXYVertices(outPlg)
# ax.plot(x,y,'tab:red')
# x,y= getXYVertices(inPlg)
# ax.plot(x,y,'tab:blue')
# plt.show()

