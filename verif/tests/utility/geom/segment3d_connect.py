# -*- coding: utf-8 -*-
''' Home made test: segment concatenation.'''

import xc_base
import geom

s1= geom.Segment3d(geom.Pos3d(0,0,0),geom.Pos3d(1,0,0))
s2= geom.Segment3d(geom.Pos3d(0,0,0),geom.Pos3d(0,1,0))
s3= geom.Segment3d(geom.Pos3d(1,0,0),geom.Pos3d(1,1,0))

s4= geom.Segment3d(geom.Pos3d(2,2,0),geom.Pos3d(-1,2,0))
s5= geom.Segment3d(geom.Pos3d(2,2,0),geom.Pos3d(2,-1,0))
s6= geom.Segment3d(geom.Pos3d(-1,2,0),geom.Pos3d(-1,-1,0))

polylines= geom.get_3d_polylines([s1,s2,s3,s4,s5,s6],.01)
ratio1= abs(polylines[0].getLength()-3)/3.0
ratio2= abs(polylines[1].getLength()-9)/9.0

'''
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
