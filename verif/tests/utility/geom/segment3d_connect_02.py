# -*- coding: utf-8 -*-
''' Home made test: segment concatenation.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc_base
import geom

p0= geom.Pos3d(0.0, 0.0, 0.0)
p1= geom.Pos3d(1.0, 0.0, 0.0)

p2= geom.Pos3d(1.0, 0.0, 0.0)
p3= geom.Pos3d(1.0, 1.0, 0.0)

p4= geom.Pos3d(1.0, 1.0, 0.0)
p5= geom.Pos3d(0.0, 1.0, 0.0)

p6= geom.Pos3d(0.0, 1.0, 0.0)
p7= geom.Pos3d(0.0, 0.0, 0.0)

s1= geom.Segment3d(p0,p1)
s2= geom.Segment3d(p2,p3)
s3= geom.Segment3d(p4,p5)
s4= geom.Segment3d(p6,p7)

p8= geom.Pos3d(0.25, 0.0, 0.0)
p9= geom.Pos3d(0.75, 0.0, 0.0)

p10= geom.Pos3d(0.75, 0.0, 0.0)
p11= geom.Pos3d(0.75, 0.5, 0.0)

p12= geom.Pos3d(0.75, 0.5, 0.0)
p13= geom.Pos3d(0.25, 0.0, 0.0)

s5= geom.Segment3d(p8,p9)
s6= geom.Segment3d(p10,p11)
s7= geom.Segment3d(p12,p13)

p14= geom.Pos3d(2.0, 0.0, 0.0)
p15= geom.Pos3d(2.0, 2.0, 0.0)

s8= geom.Segment3d(p14,p15)

segments= [s1, s2, s3, s4, s5, s6, s7, s8]

polylines= geom.get_3d_polylines(segments,.01)

ratio1= len(polylines)-3

totalLength= polylines[0].getLength()+polylines[1].getLength()+polylines[2].getLength()
refLength= 4.0+(1.0+math.sqrt(2*0.5**2))+2.0

ratio2= abs(totalLength-refLength)/refLength

'''
print('number of edges: ', len(segments))
print('number of wires: ',len(polylines))
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
for pl in polylines:
    print('pline: ', pl)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


