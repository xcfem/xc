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

pt= geom.Pos2d(-110, -10)
vertex= geom.Pos2d(-10, 0)

parabola= pb.define_generalized_parabola_from_vertex_and_point(ref= ref, vertex= vertex, pt= pt)

err= 0.0
for p in [pt, vertex, geom.Pos2d(pt.x, -pt.y)]:
    q= parabola.y(p)
    err+= q.dist2(p)

err= math.sqrt(err)
testOK= (err<1e-4)

# print("err= ", err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
