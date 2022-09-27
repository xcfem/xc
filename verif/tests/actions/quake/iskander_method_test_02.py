# -*- coding: utf-8 -*-
'''Verification of Iskander method routines.

Test based on the results in table 3 of the article: 

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
psi= math.radians(90-20) #back face inclination of the structure (<= PI/2)
phi= math.radians(20.0) #angle of internal friction of soil.
c= 0.0 # cohesion
beta= math.radians(15.0) #slope inclination of backfill.

earthPressure= earth_pressure.IskanderPressureDistribution(zGround,gammaSoil= gammaSoil,H= H,kv= 0.0,kh= 0.0,psi= psi,phi= phi,beta= beta, c= c)

# Compute static active lateral earth pressure coefficient for cohesionless soil with vertical wall.
Pi_aster= list()
factor= gammaSoil*H*H
for phi in [35, 40]:
    earthPressure.setPhi(math.radians(phi))
    for kh in [0.1, 0.2, 0.3]:
        earthPressure.setKh(kh)
        P= earthPressure.iskander.P_AEH2() 
        Pi_aster.append(P/factor)

refPi_aster= [0.2295881040688732, 0.32620028993814315, 0.5014800104236178, 0.1840741011132839, 0.26075842920841935, 0.38101503071286497]

err= 0.0
for v1, v2 in zip(Pi_aster, refPi_aster):
    err+= (v1-v2)**2
err= math.sqrt(err)

'''
print('Pi_aster= ', Pi_aster)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
