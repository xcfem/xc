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

s1= geom.Segment3d(geom.Pos3d(0,0,0),geom.Pos3d(1,0,0))
s2= geom.Segment3d(geom.Pos3d(0,0,0),geom.Pos3d(0,1,1))
s3= geom.Segment3d(geom.Pos3d(1,0,0),geom.Pos3d(1,1,1))

s4= geom.Segment3d(geom.Pos3d(2,2,0),geom.Pos3d(-1,2,0))
s5= geom.Segment3d(geom.Pos3d(2,2,0),geom.Pos3d(2,-1,0))
s6= geom.Segment3d(geom.Pos3d(-1,2,0),geom.Pos3d(-1,-1,0))

polylines= geom.get_3d_polylines([s1,s2,s3,s4,s5,s6],.01)
pl1RefLength= 2.0*math.sqrt(2)+1
ratio1= abs(polylines[0].getLength()-pl1RefLength)/pl1RefLength
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
