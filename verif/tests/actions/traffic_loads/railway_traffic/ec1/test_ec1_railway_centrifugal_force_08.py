# -*- coding: utf-8 -*-
''' Test values of centrifugal load according to clause 6.5.1 of EC1-2 clause 6.6. Check centrifugal loads of the SW/0 load model.

'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import EC1_rail_load_models


V= 350 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)

Lf=  150.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural elenlent under consideration [m].

dynamicFactor= 0.987

r= 3507 # radius of curvature.

# Compute centrifugal forces using the train load model.
locomotiveSW0= EC1_rail_load_models.get_phantom_locomotive_SW0() # Phantom locomotive.
sw0= EC1_rail_load_models.TrainLoadModel(locomotive= locomotiveSW0, uniformLoad= 133e3, dynamicFactor= dynamicFactor, classificationFactor= 1.1)
(Qtk_sw0, qtk_sw0)= sw0.getCentrifugalForces(v= v, Lf= Lf, r= r)

# Check results.
qtk_ratio= abs(qtk_sw0-12.627328642671168e3)/12.627328642671168e3
Qtk_ratio= abs(Qtk_sw0)

'''
print('Reduction factor of the centrifugal force: f= ', f)
print('qtk_sw0= ', qtk_sw0/1e3, 'kN/m')
print('Qtk_sw0= ', Qtk_sw0/1e3, 'kN/axis')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(qtk_ratio<1e-6 and Qtk_ratio<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


