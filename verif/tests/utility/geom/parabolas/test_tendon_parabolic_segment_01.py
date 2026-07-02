# -*- coding: utf-8 -*-
''' Trivial test of tendon parabolic segment function.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geom_utils import parabola as pb

ft= .3048

vertex= geom.Pos2d(0, 0.0)
e_max= 2.5*ft
e_min= -2.0*ft # There is an error in the document.

h= e_max
b= 48*ft

parabola= pb.def_tendon_parabolic_segment(vertex= vertex, b=b, h= h)

y= parabola.y(-b)
ratio1= abs(y-h)/h

# print(y/ft, ratio1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display parabola.
# import matplotlib.pyplot as plt
# import numpy as np
# xi= np.linspace(start= -b, stop= 0.0, num= 20)
# yi= list()
# for x in xi:
#     xf= float(x)
#     yi.append(parabola.y(xf))
# plt.plot(xi, yi, '-')
# plt.grid()
# plt.show()
