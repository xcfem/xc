# -*- coding: utf-8 -*-
''' Test getVerticalPressureDistribution for IAP-11.'''

from __future__ import print_function
from __future__ import division

import math
from actions.wind import IAP_wind

xA= -1.4 # leftmost point.
xB= 1.4 # rightmost point.
(verticalPressure0,verticalPressure1)= IAP_wind.getVerticalPressureDistribution(terrainCategory= 'III', x0= xA, x1= xB, z= 12.0, v_b= 29)

xi= [-1.4, -1.0, -0.8, 0.0, 0.8, 1.0, 1.4]

p0= list()
p1= list()
for x in xi:
    p0.append(float(verticalPressure0(x)))
    p1.append(float(verticalPressure1(x)))

R0= (p0[-1]+p0[0])*(xi[-1]-xi[0])/2.0
R1= (p1[-1]+p1[0])*(xi[-1]-xi[0])/2.0
ratio1= abs(R0+R1)

vForce= IAP_wind.getVerticalForce(terrainCategory= 'III', z= 12.0, v_b= 29, A_refz= xB-xA)
ratio2= math.sqrt((vForce[0]-R0)**2+(vForce[1]-R1)**2)

'''
print('p0= ', p0)
print('R0= ', R0)
print('p1= ', p1)
print('R1= ', R1)
print("ratio1= ", ratio1)
print('vForce= ', vForce)
print("ratio2= ", ratio2)
'''

'''
import matplotlib.pyplot as plt
plt.plot(xi,p0, label='downwards')
plt.plot(xi,p1, label='upwards')
plt.xlabel('distance (m)')
plt.ylabel('Pressure (Pa)')
plt.legend() # Add a legend.
plt.grid() # Draw grid.
plt.show()
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if( (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) ):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




