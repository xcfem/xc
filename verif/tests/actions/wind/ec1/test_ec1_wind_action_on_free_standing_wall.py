# -*- coding: utf-8 -*-
''' Compute the values of the recommendend pressure coefficients for
    a free standing wall.

Valued checked against the results obtained from:

https://eurocodeapplied.com/design/en1991/wind-pressure-freestanding-wall
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.wind import ec1_wind

# Compute the value of the probability factor.
c_prob= ec1_wind.get_probability_factor(T= 5)

c_dir= 1.0
c_season= 1.0
# Compute basic velocity
vb0= 26
vb= c_prob*c_dir*c_season*vb0
rho= 1.25


cscd= 1.0 # structural factor.

# Compute peak velocity pressure.
terrainCategory= 'III'
wallLength= 2.0*5 # Length of the wall.
wallHeight= 2.0 # Height of wall above ground.
hBase= 8.0 # Distance of the base of the wall from the ground.
lCorner= 0.0 # Length of return corner.
solidityRatio= 1.0 # Solidity ratio.
c0= 1.0 # orography factor.
rho= 1.25 # air density.
k1= 1.0 # turbulence factor.
zMax= 200.0
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= hBase+wallHeight, zMax= zMax, rho= rho, k1= k1, c0= c0)

# Compute pressure distribution.
abcd= ec1_wind.get_free_standing_wall_abcd_cp(length= wallLength, height= wallHeight, returnCornersLength= lCorner, solidityRatio= solidityRatio)
abcdRef= [2.9, 1.8, 1.4, 1.2]

# Mean square error.
errCp= 0.0
for v1, v2 in zip(abcd, abcdRef):
    errCp+= (v2-v1)**2
errCp= math.sqrt(errCp)


# Pressure distribution.
wNet= ec1_wind.get_free_standing_wall_net_pressure_distribution(terrainCategory= terrainCategory, vb= vb, length= wallLength, height= wallHeight, hBase= hBase, zMax= zMax, rho= rho, k1= k1, c0= c0, returnCornersLength= lCorner, solidityRatio= solidityRatio)
wnet_i= list()
for x in [0.15*wallHeight, wallHeight, 3*wallHeight, 4.5*wallHeight]:
    wnet_i.append(float(wNet(x)))

refWnet= [1530, 949, 738, 633]
# Root mean square relative error.
errWnet= 0.0
for v1, v2 in zip(wnet_i, refWnet):
    errWnet+= ((v2-v1)/v2)**2
errWnet/=len(refWnet)
errWnet= math.sqrt(errWnet)

'''
print('Basic wind velocity= ', vb, ' m/s')
print('Terrain category: ', terrainCategory)
print('Wall length: l= ', wallLength, ' m')
print('Height of wall above ground: h= ',wallHeight, ' m')
print('Distance of the base of the wall from the ground: ',hBase, ' m')
print('Length of return corner: ',lCorner, ' m')
print('Solidity ratio: ',solidityRatio)
print('Orography factor: ', c0)
print('Air density: ', rho)
print('Peak velocity pressure: ', qp/1e3, 'kN/m2')
print('Net pressures: ', wnet_i, 'kN/m2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(errCp)<1e-6) and (abs(errWnet)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

