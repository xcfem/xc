# -*- coding: utf-8 -*-
''' Trivial test of tendon parabolic segment function.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geom_utils import parabola as pb

ft= .3048

vertex= geom.Pos2d(60*ft, 3.75*ft)
e_max= 2.5*ft
e_min= -2.0*ft # There is an error in the document.

h= -0.75*ft
b= 12*ft


parabola= pb.def_tendon_parabolic_segment(vertex= vertex, b=b, h= h)

y1= parabola.y(vertex.x-b)
y1Ref= vertex.y+h
ratio1= abs(abs(y1-y1Ref)/y1Ref)
y2= parabola.y(vertex.x)
y2Ref= vertex.y
ratio2= abs(abs(y2-y2Ref)/y2Ref)

'''
print(y1/ft, y1Ref/ft, ratio1)
print(y2/ft, y2Ref/ft, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display tendon parabolic segment.
# import matplotlib.pyplot as plt
# import numpy as np
# xi= np.linspace(start= vertex.x-b, stop= vertex.x, num= 20)
# yi= list()
# for x in xi:
#     xf= float(x)
#     yi.append(parabola.y(xf))
# plt.plot(xi, yi, '-')
# plt.grid()
# plt.show()
