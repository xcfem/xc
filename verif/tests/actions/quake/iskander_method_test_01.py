# -*- coding: utf-8 -*-
'''Verification of Iskander method routines.

Test based on the results in table 1 of the article: 

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
gammaSoil= 21e3 #N/m3 unit weight of soil (backfill)
H= 4 #m height of the structure.
psi= math.radians(90) #back face inclination of the structure (<= PI/2)
phi= math.radians(20.0) #angle of internal friction of soil.
c= 0.0 # cohesion
beta= 0.0 #slope inclination of backfill.

earthPressure= earth_pressure.IskanderPressureDistribution(zGround,gammaSoil= gammaSoil,H= H,kv= 0.0,kh= 0.0,psi= psi,phi= phi,beta= beta, c= c)

# Compute static active lateral earth pressure coefficient for cohesionless soil with vertical wall.
hl= earthPressure.iskander.Hl()
sgV= gammaSoil*hl # Vertical pressure at wall heel.
Ka= list()
for phi in [20, 30, 40]:
    earthPressure.setPhi(math.radians(phi))
    sgH= earthPressure.iskander.sigma_AEH(hl) # Horizontal pressure at wall heel.
    Ka.append(sgH/sgV)

refKa= [0.4902905965657023, 0.33333333333333326, 0.21744283205399895]
err= 0.0
for ka1, ka2 in zip(Ka, refKa):
    err+= (ka1-ka2)**2
err= math.sqrt(err)

'''
print('Ka= ', Ka)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
