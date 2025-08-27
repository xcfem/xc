# -*- coding: utf-8 -*-
''' Simple test of the Parabola objects.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geom_utils import parabola as pb

vertex= geom.Pos2d(0.0,0.0)
pt= geom.Pos2d(10,100)

parabola= pb.define_parabola_from_vertex_and_point(vertex= vertex, pt= pt)

err= 0
for p in [vertex, pt, geom.Pos2d(-pt.x, pt.y)]:
    err+= (p.y-parabola.y(p.x))**2

err= math.sqrt(err)

# print("err= ", err)
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display parabola.
# import matplotlib.pyplot as plt
# xi= list()
# yi= list()
# for i in range(0,41):
#     x= p0.x+float(i)/40*(p2.x-p0.x)
#     xi.append(x)
#     yi.append(parabola.y(x))
# plt.plot(xi, yi, '-')
# plt.grid()
# plt.show()
