# -*- coding: utf-8 -*-
'''Polyline2d::offset test for closed polylines. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

points= [geom.Pos2d(7489.38, 5463.23), geom.Pos2d(7391.43, 5436.11), geom.Pos2d(7384.67, 5460.99), geom.Pos2d(7525.98, 5501.24), geom.Pos2d(7533.25, 5476.79), geom.Pos2d(7489.38, 5463.23)]

pline= geom.Polyline2d(points)
offsetIn= pline.offset(5)
offsetOut= pline.offset(-5)

# Check results.
inLength= offsetIn.getLength()
inLengthRef= 306.0680801729298
outLength= offsetOut.getLength()
outLengthRef= 385.47884665333413
err= math.sqrt((inLengthRef-inLength)**2+(outLengthRef-outLength)**2)

'''
print(pline.isClosed(1e-6))
print(inLength)
print(outLength)
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
# x,y= getXYVertices(pline)
# ax.plot(x,y,'tab:gray')
# x,y= getXYVertices(offsetIn)
# ax.plot(x,y,'tab:red')
# x,y= getXYVertices(offsetOut)
# ax.plot(x,y,'tab:green')
# plt.show()
