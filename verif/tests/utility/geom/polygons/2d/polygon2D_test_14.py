# -*- coding: utf-8 -*-
'''Clip a segment by a polygon. Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

points= [geom.Pos2d(7364.55, 5454.78), geom.Pos2d(7369.1516, 5455.0753), geom.Pos2d(7373.7491, 5455.3703), geom.Pos2d(7378.3466, 5455.6653), geom.Pos2d(7382.94, 5455.96), geom.Pos2d(7387.2994, 5456.2831), geom.Pos2d(7392.3924, 5456.6605), geom.Pos2d(7397.4777, 5457.0374), geom.Pos2d(7402.5671, 5457.4152), geom.Pos2d(7405.0385, 5457.626), geom.Pos2d(7407.6444, 5457.8482), geom.Pos2d(7412.7208, 5458.2809), geom.Pos2d(7416.149, 5458.5736), geom.Pos2d(7422.8741, 5459.1486), geom.Pos2d(7427.9334, 5459.6454), geom.Pos2d(7432.9914, 5460.142), geom.Pos2d(7438.0451, 5460.6382), geom.Pos2d(7442.85, 5461.11), geom.Pos2d(7448.1365, 5461.7024), geom.Pos2d(7453.1767, 5462.2673), geom.Pos2d(7460.34, 5463.07), geom.Pos2d(7462.83, 5463.37), geom.Pos2d(7467.1584, 5463.9255), geom.Pos2d(7472.1741, 5464.5692), geom.Pos2d(7477.192, 5465.2131), geom.Pos2d(7482.7, 5465.92), geom.Pos2d(7487.2018, 5466.5709), geom.Pos2d(7490.7968, 5467.0907), geom.Pos2d(7494.6908, 5467.6537), geom.Pos2d(7495.4188, 5465.0965), geom.Pos2d(7499.9574, 5466.4994), geom.Pos2d(7506.1616, 5468.4172), geom.Pos2d(7512.5669, 5470.3971), geom.Pos2d(7517.4684, 5471.1836), geom.Pos2d(7522.37, 5471.97), geom.Pos2d(7527.2476, 5472.8308), geom.Pos2d(7532.1927, 5473.7034), geom.Pos2d(7537.67, 5474.67), geom.Pos2d(7536.25, 5483.05), geom.Pos2d(7530.7153, 5482.0752), geom.Pos2d(7525.77, 5481.2042), geom.Pos2d(7521.09, 5480.38), geom.Pos2d(7516.1528, 5479.5851), geom.Pos2d(7511.2156, 5478.7903), geom.Pos2d(7508.5913, 5478.3678), geom.Pos2d(7504.8094, 5477.7834), geom.Pos2d(7498.4721, 5476.7873), geom.Pos2d(7493.4763, 5476.066), geom.Pos2d(7489.5822, 5475.5038), geom.Pos2d(7485.9871, 5474.9847), geom.Pos2d(7481.66, 5474.36), geom.Pos2d(7476.1066, 5473.6451), geom.Pos2d(7471.089, 5472.9993), geom.Pos2d(7466.0735, 5472.3537), geom.Pos2d(7461.85, 5471.81), geom.Pos2d(7459.45, 5471.52), geom.Pos2d(7452.2338, 5470.7145), geom.Pos2d(7447.1934, 5470.1519), geom.Pos2d(7442.07, 5469.58), geom.Pos2d(7437.21, 5469.1004), geom.Pos2d(7432.1565, 5468.6016), geom.Pos2d(7427.0987, 5468.1025), geom.Pos2d(7422.21, 5467.62), geom.Pos2d(7415.4269, 5467.0402), geom.Pos2d(7414.7559, 5469.3964), geom.Pos2d(7411.9536, 5468.6323), geom.Pos2d(7406.9464, 5467.2671), geom.Pos2d(7401.8852, 5465.8871), geom.Pos2d(7396.8561, 5465.5128), geom.Pos2d(7391.7631, 5465.1346), geom.Pos2d(7386.6701, 5464.7571), geom.Pos2d(7382.4, 5464.44), geom.Pos2d(7377.8025, 5464.145), geom.Pos2d(7373.205, 5463.85), geom.Pos2d(7368.6075, 5463.555), geom.Pos2d(7364.01, 5463.26)]

plg= geom.Polygon2d(points)

testSegment= geom.Segment2d(geom.Pos2d(7489.13, 5489.18), geom.Pos2d(7495.53, 5466.7))
clippedSegment= plg.clip(testSegment)

pa= clippedSegment.getFromPoint()

# Check results.
exists= clippedSegment.exists
ratio1= abs(clippedSegment.getLength()-9.649823194495923)/9.649823194495923
refPoint= geom.Pos2d(7492.8877, 5475.981)
ratio2= refPoint.dist(pa)

'''
print(clippedSegment.getLength(), ratio1)
print(pa, ratio2, clippedSegment.exists)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if exists and abs(ratio1)<1e-8 and abs(ratio2)<1e-4:
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
# pA= testSegment.getFromPoint()
# pB= testSegment.getToPoint()
# xi= [pA.x, pB.x]
# yi= [pA.y, pB.y]
# plt.plot(xi, yi, 'tab:blue')
# pa= clippedSegment.getFromPoint()
# pb= clippedSegment.getToPoint()
# xi= [pa.x, pb.x]
# yi= [pa.y, pb.y]
# plt.plot(xi, yi, 'tab:red')
# plt.show()
