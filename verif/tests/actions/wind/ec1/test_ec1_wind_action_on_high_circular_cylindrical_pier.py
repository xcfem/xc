# -*- coding: utf-8 -*-
''' Compute the wind force on a high cylindrical pier.

Based on the example 3.3.1 of the publication Bridge Design to Eurocodes
Worked examples. European Commission 2012. ISBN 978-92-79-22823-0
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
from scipy.integrate import quad

# Compute the value of the probability factor.
c_prob= ec1_wind.get_probability_factor(T= 100)
ratio0= abs(c_prob-1.0384765480213738)/1.0384765480213738

c_dir= 1.0
c_season= 1.0
# Compute basic velocity
vb0= 26
vb= c_prob*c_dir*c_season*vb0
rho= 1.25

cscd= 1.0 # structural factor.

# Compute peak velocity pressure.
terrainCategory= 'II'
z= 40.0 # High pier
zMax= 200.0
k1= 1.0 # turbulence factor.
c0= 1.0 # orography factor
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
qpRef= 1504.6516580253199
ratio1= abs(qp-qpRef)/qpRef

# Compute the force coefficient.
b= 4.0 # pier diameter.
l= 40.0 # pier height.
k= 0.2e-3 # smooth concrete see table 7.13 of EN 1991-1-4:2005
cf0= ec1_wind.get_cylinder_force_coefficient_without_free_end_flow(b= b, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0, nu=15e-6)
ratio2= abs(cf0-0.7892818551764009)/0.7892818551764009

# end-effect factor (for elements with free-end flow [see 7.13 on EN 1991-1-4:2005])
psi_A= ec1_wind.get_cylinder_end_effect_factor(b= b, l= l, solidityRatio= 1.0)
ratio3= abs(psi_A-0.6877359356490387)/0.6877359356490387

cf= ec1_wind.get_cylinder_force_coefficient(b= b, l= l, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0, nu= 15e-6, solidityRatio= 1.0)
cfRef= cf0*psi_A
ratio4= abs(cf-cfRef)/cfRef


# Compute the wind force (integrate the pressures along the pier).
Aref= b*l
Fw= cscd*cf*qp*Aref # qp(z=40); conservative approach.
ratio5= abs(Fw-130.68019905575588e3)/130.68019905575588e3

'''
print('qp= ', qp, ratio1)
print('cf0= ', cf0, ratio2)
print('psi_A= ', psi_A, ratio3)
print('cf0= ', cf, ratio4)
print('Fw= ', Fw/1e3, 'kN', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-4) and (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4) and (abs(ratio4)<1e-4) and (abs(ratio5)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Plot pressures along the pier height.
# import matplotlib.pyplot as plt
# import numpy as np
# xi= np.arange(0, l, 0.5)
# zi= pz(xi)   # use interpolation function returned by `interp1d`
# plt.plot(zi, xi, '-')
# plt.show()
