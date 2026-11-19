# -*- coding: utf-8 -*-
'''Trivial test on earth pressure increment due to a vertical line load on the
   backfill of a retaining wall.

Reference: Calavera Ruiz, J., and J. Calavera. Muros de contención y muros de sótano. España: Intemac, 1987. ISBN 9788439890928, 8439890923.
'''

from __future__ import print_function
from __future__ import division


__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import math
from actions.earth_pressure import earth_pressure as ep

# qLoad: load value (F/L).
# zLoad: global Z coordinate where the line load acts.
# distWall: horizontal distance between the wall and the line surcharge load.

q= 60e3 # N/m
zLoad= 0.0 # m
z= -3 # m
z_rel= (zLoad-z) # m
x= 2.0 # m
denom= math.pi*z_rel
omega= math.atan2(x, z_rel)
num= q*(math.sin(2*omega))**2
deltaPRef= num/denom

lineVerticalLoad= ep.LineVerticalLoadOnBackfill(qLoad= 60e3, zLoad= 0.0, distWall= x)

deltaP= lineVerticalLoad.getPressure(z= -3.0)
ratio1= abs(deltaP-deltaPRef)/deltaPRef

'''
print(deltaP/1e3, 'kPa')
print(ratio1)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# import numpy as np
# zi= np.linspace(0.0, -9.0, num=50)
# pi= list()
# for z in zi:
#     pi.append(lineVerticalLoad.getPressure(z))
# print(zi)
# print(pi)

# import matplotlib.pyplot as plt
# yi= list()
# xi= list()
# for z, p in zip(zi, pi):
#     yi.append(z)
#     xi.append(p/1e3)
    
# plt.plot(xi, yi)
# plt.show()
