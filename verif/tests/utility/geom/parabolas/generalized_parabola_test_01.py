# -*- coding: utf-8 -*-
''' Simple test of the Parabola objects.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geom_utils import parabola as pb


org= geom.Pos2d(-10,0)
vI=geom.Vector2d(0, -1)
ref= geom.Ref2d2d(org,vI)

p0= geom.Pos2d(-110, -10)
p1= geom.Pos2d(-10, 0)
p2= geom.Pos2d(-110, +10)

parabola= pb.GeneralizedParabola(ref, p0, p1, p2)

err= 0.0
for p in [p0, p1, p2]:
    q= parabola.y(p)
    err+= q.dist2(p)


curvatures= list()
for p in [p0, p1, p2]:
    c= parabola.curvature(p)
    curvatures.append(c)
err+= (curvatures[0]-curvatures[2])**2
err+= (curvatures[1]+2)**2

err= math.sqrt(err)
testOK= (err<1e-4)

'''
print("err= ", err)
print(curvatures)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
