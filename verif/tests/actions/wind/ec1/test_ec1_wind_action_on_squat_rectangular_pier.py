# -*- coding: utf-8 -*-
''' Compute the wind force on a squat rectangular pier.

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
z= 10.0 # Squat pier
zMax= 200.0
k1= 1.0 # turbulence factor.
c0= 1.0 # orography factor
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
qpRef= 1071.7931242585819
ratio1= abs(qp-qpRef)/qpRef

# Compute the force coefficient.
d= 5.0 # pier length.
b= 2.5 # pier width.
cf0= ec1_wind.get_rectangular_section_force_coefficient_without_free_end_flow(d= d, b= b)
ratio2= abs(cf0-1.65)/1.65

# reduction factor for a square cross-section with rounded corners
r= 0.0
psi_r= ec1_wind.get_square_section_round_corners_reduction_factor(b= b, r= r)
ratio3= abs(psi_r-1)

# end-effect factor (for elements with free-end flow [see 7.13 on EN 1991-1-4:2005])
l= 10.0 # pier height.
psi_l= ec1_wind.get_polygonal_section_end_effect_factor(b= b, l= l, solidityRatio= 1.0)
ratio4= abs(psi_l-0.6885)/0.6885

# Compute force coefficient for rectangular section.
cf= ec1_wind.get_rectangular_section_force_coefficient(d= d, b= b, l= l, r= r, solidityRatio= 1.0)
cfRef= cf0*psi_l*psi_r
ratio5= abs(cf-cfRef)/cfRef

# Compute the pressure distribution on the pier.
pz, zi= ec1_wind.get_rectangular_prism_pressure_distribution(d=d, b= b, h= l, terrainCategory= terrainCategory, vb= vb, zMax= zMax, rho= rho, k1= k1, c0= c0, cscd= 1.0, r= r)

# Compute the wind force (integrate the pressures along the pier).
(Fw, error)= quad(pz, zi[0], zi[-1], points= zi)
Fw*=b # multiply by the pile width.
ratio6= abs(Fw-25.190743398944903e3)/25.190743398944903e3

'''
print('qp= ', qp, ratio1)
print('cf0= ', cf0, ratio2)
print('psi_r= ', psi_r, ratio3)
print('psi_l= ', psi_l, ratio4)
print('cf= ', cf, ratio5)
print('Fw= ', Fw/1e3, 'kN', ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-4) and (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4) and (abs(ratio4)<1e-4) and (abs(ratio5)<1e-4) and (abs(ratio6)<1e-4):
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
