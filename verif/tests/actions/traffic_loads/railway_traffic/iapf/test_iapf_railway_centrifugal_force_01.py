# -*- coding: utf-8 -*-
''' Test values of centrifugal force according to clause 2.3.2.2 of IAPF. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import IAPF_rail_load_models

V= 160 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)

Lf=  10.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural elenlent under consideration [m].

# Compute reduction factor of the centrifugal force according to
# expression 6.19 of Eurocode 1 part 2 (clause 6.5.1 paragraph 8).
f= IAPF_rail_load_models.centrifugal_force_reduction_factor(v= v, Lf= Lf)

# characteristic value of the vertical concentrated load 
# specified in clause 6.3 (v>120 km/h => alpha= 1.0)
alpha= 1.0
Qvk= 250e3*alpha # per axis (3 axis on the bridge)

# characteristic value of the vertical distributed load 
# specified in clause 6.3 (v>120 km/h => alpha= 1.0)
qvk= 80e3*alpha # per meter

r= 1280 # radius of curvature.

# Compute centrifugal forces.
(Qtk, qtk)= IAPF_rail_load_models.get_centrifugal_forces(v= v, Lf= Lf, r= r, Qvk= Qvk, qvk= qvk)


# Check results.
ratio1= abs(f-0.8732755020430862)/0.8732755020430862
QtkRef= 34.355447061333905e3
ratio2= abs(Qtk-QtkRef)/QtkRef
qtkRef= 10.99374305962685e3
ratio3= abs(qtk-qtkRef)/qtkRef

# Print results.
'''
print('Reduction factor of the centrifugal force: f= ', f)
print('ratio1= ', ratio1)
print('Characteristic value of the concentrated centrifugal force: Qtk= ', Qtk/1e3, 'kN/axis')
print('ratio2= ', ratio2)
print('Characteristic value of the distributed centrifugal force: qtk= ', qtk/1e3, 'kN/m')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
