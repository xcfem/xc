# -*- coding: utf-8 -*-
'''Verification of Iskander method routines.

Test based on the results shown in figure 5 of the article: 

Active static and seismic earth pressure for c–φ soils Magued Iskander, 
Zhibo (Chris) Chen , Mehdi Omidvar, Ivan Guzman, Omar Elsheri. 
Polytechnic Institute of New York University, USA.
'''


from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.earth_pressure import earth_pressure

zGround= 0.0 #m z coordinate of the ground surface.
gammaSoil= 23e3 #N/m3 unit weight of soil (backfill)
H= 15 # m height of the structure.
kh= 0.2 # seismic coefficient of horizontal acceleration.
kv= -kh/2.0 # seismic coefficient of vertical acceleration.
psi= math.radians(90-20) # back face inclination of the structure (<= PI/2)
phi= math.radians(30.0) # angle of internal friction of soil.
c= 20.0e3 # cohesion
beta= math.radians(15) # slope inclination of backfill.

# # Compute reference value.
# from geotechnics import iskander_method
# omega= math.radians(90)-psi
# iskanderModelEq= iskander_method.Iskander(kh= kh, kv= kv, omega= omega, beta= beta, phi= phi, gamma= gammaSoil, c= c, H= H)
# iskanderModelNoEq= iskander_method.Iskander(kh= 0.0, kv= 0.0, omega= omega, beta= beta, phi= phi, gamma= gammaSoil, c= c, H= H)
# hl= iskanderModelEq.Hl()
# refPressureA= iskanderModelEq.sigma_AEH(hl)
# refPressureB= iskanderModelNoEq.sigma_AEH(hl)
# refPressureC= refPressureA-refPressureB
# print('omega= ', math.degrees(omega))
# print('hl= ', hl)
# print('pressure with earthquake: ', refPressureA/1e3, ' kN/m2')
# print('pressure without earthquake: ', refPressureB/1e3, ' kN/m2')
# print('difference: ',refPressureC/1e3, ' kN/m2')

hl=  15.962666587138683
refPressure= 111.37199281539398e3

earthPressure= earth_pressure.IskanderPressureDistribution(zGround= zGround,gammaSoil= gammaSoil,H= H,kv= kv,kh= kh,psi= psi,phi= phi,beta= beta, c= c)

sigmaMin= earthPressure.getPressure(0.0)
sigma= earthPressure.getPressure(-hl/2.0)
sigmaMax= earthPressure.getPressure(-hl)

ratio1= (sigmaMax-refPressure)/refPressure
ratio2= (sigma-sigmaMax/2.0)/(sigmaMax/2.0)
ratio3= sigmaMin

'''
print('max_stress= ', earthPressure.max_stress/1e3, ' kN/m2')
print('sigmaMax= ', sigmaMax/1e3, ' kN/m2')
print('ratio1= ', ratio1 )
print('sigma= ', sigma/1e3, ' kN/m2')
print('ratio2= ', ratio2 )
print('sigmaMin= ', sigmaMin/1e3, ' kN/m2')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
