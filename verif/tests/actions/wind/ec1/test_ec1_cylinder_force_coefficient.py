# -*- coding: utf-8 -*-
''' Compute the wind force coefficient on a circular cylinder according to
the expressions in the figurea 7.28 of EN 1991-1-4:2005.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from actions.wind import ec1_wind

# Compute the value of the force coefficient without free-end flow.
terrainCategory= 'II'
z= 40.0 # High pier
zMax= 200.0
k1= 1.0 # turbulence factor.
c0= 1.0 # orography factor
vb= 24.033 # basic wind velocity.

b= 0.4
l= 15
k= .2e-3
cf0= ec1_wind.get_cylinder_force_coefficient_without_free_end_flow(b=b, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= 1.25, k1= k1, nu= 15e-6)
cf0Ref= 0.7964968861801018
ratio1= abs(cf0-cf0Ref)/cf0Ref

endEffectFactor= ec1_wind.get_cylinder_end_effect_factor(b= b, l= l, solidityRatio= 1.0)
endEffectFactorRef= 0.8475704393676162
ratio2= abs(endEffectFactor-endEffectFactorRef)/endEffectFactorRef

cf= ec1_wind.get_cylinder_force_coefficient(b=b, l= l, k= k, terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= 1.25, k1= k1, nu= 15e-6, solidityRatio= 1.0)
cfRef= cf0Ref*endEffectFactorRef
ratio3= abs(cf-cfRef)/cfRef

w_eff= ec1_wind.get_cylinder_effective_wind_pressure(terrainCategory= 'II', vb= 27.0, z= l, b= b, l= l, k= .2e-3, zMax= 200.0, rho= 1.25, k1= 1.0, c0= 1.0, nu= 15e-6, solidityRatio= 1.0, structuralFactor= 1.0)
w_effRef= 804.5745670535356
ratio4= abs(w_eff-w_effRef)/w_effRef

'''
print("cf0= ", cf0)
print("ratio1= ", ratio1)
print('endEffectFactor= ', endEffectFactor)
print("ratio2= ", ratio2)
print("cf= ", cf)
print("ratio3= ", ratio3)
print("w_eff= ", w_eff)
print("ratio4= ", ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4) and (abs(ratio4)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
