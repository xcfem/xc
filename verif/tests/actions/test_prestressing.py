# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from geom_utils import parabola as pb
import math

e1= 0.5
e2= 0.0
f=0.25
L= 10.0
P= 10

uTeor= 8*P*f/L**2

p0= geom.Pos2d(0,e1)
p1= geom.Pos2d(L/2.0,(e1+e2)/2.0-f)
p2= geom.Pos2d(L,e2)

traceCable= pb.Parabola(p0,p1,p2)
 
x= 0.0
err= 0
nPts= 5
for i in range(1,nPts+2):
    x= float(i-1)*L/float(nPts)
    alpha= traceCable.alpha(x)
    cosAlpha= math.cos(alpha)
    r= traceCable.curvature(x)
    Px= P/cosAlpha*geom.Vector2d(cosAlpha,math.sin(alpha))
    u= (P/cosAlpha)*r
    err+= (u-uTeor)**2

err= math.sqrt(err)

# print("err= ", err)
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display parabola.
# import matplotlib.pyplot as plt
# xi= list()
# yi= list()
# for i in range(0,41):
#     x= float(i)/40*L
#     xi.append(x)
#     yi.append(traceCable.y(x))
# plt.plot(xi, yi, '-')
# plt.grid()
# plt.show()
