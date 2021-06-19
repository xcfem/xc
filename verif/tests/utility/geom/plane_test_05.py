from __future__ import print_function 
import xc_base
import geom
p0=geom.Pos3d(0.000331586,0,0)
# p1=geom.Pos3d(0.000331586,100,0)
# p2=geom.Pos3d(-0.301719,0,100)
p1=geom.Pos3d(0.000331586,1,0)
p2=geom.Pos3d(-0.00268892,0,1)

pA= geom.Pos3d(0.0,1,0.109779)
pB= geom.Pos3d(0.0,0,0.109779)

plane=geom.Plane3d(p0,p1,p2)
normal=plane.getNormal()
YZplane= geom.Plane3d(geom.Pos3d(0,0,0),geom.Pos3d(0,1,0),geom.Pos3d(0,0,1))
angle= plane.getAngle(YZplane)
yzTrace= plane.getYZTrace()
dA= yzTrace.dist(pA)
dB= yzTrace.dist(pB)

'''
print('normal: ', normal*(1.0/1e4))
print('angle= ', angle)
print('trace= ', yzTrace)
print('dA= ', dA)
print('dB= ', dB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(dA<1e-6 and dB<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
