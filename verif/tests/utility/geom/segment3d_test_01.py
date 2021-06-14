# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division
import xc_base
import geom
import math

A= geom.Pos3d(0.0, 0.0, 0.0)
B= geom.Pos3d(1.0, 0.0, 0.0)
C= geom.Pos3d(1.0, 1.0, 0.0)
D= geom.Pos3d(0.0, 1.0, 0.0)
E= geom.Pos3d(0.0, 0.0, 1.0)
F= geom.Pos3d(1.0, 0.0, 1.0)
G= geom.Pos3d(1.0, 1.0, 1.0)
H= geom.Pos3d(0.0, 1.0, 1.0)

a= geom.Segment3d(A,B)
b= geom.Segment3d(B,C)
c= geom.Segment3d(C,D)
d= geom.Segment3d(D,A)
e= geom.Segment3d(E,F)
f= geom.Segment3d(F,G)
g= geom.Segment3d(G,H)
h= geom.Segment3d(H,E)
# Intersection to detect
i= geom.Segment3d(A,G)
j= geom.Segment3d(D,F)

segments= geom.dq_segment3D()
segments.append(a)
segments.append(b)
segments.append(c)
segments.append(d)
segments.append(e)
segments.append(f)
segments.append(g)
segments.append(h)
segments.append(i)
segments.append(j)

pairList= geom.getSegmentIntersections(segments)

firstIntersection= pairList[0]
firstSegment= segments[firstIntersection.first]
secondSegment= segments[firstIntersection.second]
p1= firstSegment.getCenterOfMass()
p2= secondSegment.getCenterOfMass()
ratio1= len(pairList)-1
ratio2= p1.dist(p2)

'''
print('p1: '+str(p1)) 
print('p2: '+str(p2)) 
print('ratio2: '+str(ratio2)) 
for p in pairList:
    print(p.first, p.second)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(math.fabs(ratio1)<1e-10 and math.fabs(ratio2)<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
