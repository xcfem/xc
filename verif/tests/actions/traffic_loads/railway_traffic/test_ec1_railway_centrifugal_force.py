# -*- coding: utf-8 -*-
''' Test values of slipstream effect according to EC1-2 clause 6.6. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import EC1_rail_load_models

V= 200 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)

Lf=  5.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural elenlent under consideration [m].

# Compute reduction factor of the centrifugal force according to
# expression 6.19 of Eurocode 1 part 2 (clause 6.5.1 paragraph 8).
f= EC1_rail_load_models.centrifugal_force_reduction_factor(v= v, Lf= Lf)

# characteristic value of the vertical concentrated load 
# specified in clause 6.3 (alpha= 1.21)
Qvk= 250e3*1.21 # per axis (3 axis on the bridge)

# characteristic value of the vertical distributed load 
# specified in clause 6.3 (alpha= 1.21)
qvk= 80e3*1.21 # per meter

r= 2200 # radius of curvature.

# Compute centrifugal forces.
(Qtk, qtk)= EC1_rail_load_models.get_centrifugal_forces(v= v, Lf= Lf, r= r, Qvk= Qvk, qvk= qvk)

# Check results.
ratio1= abs(f-0.8877655548578554)/0.8877655548578554
ratio2= abs(Qtk-38.418048710381626e3)/38.418048710381626e3
ratio3= abs(qtk-12.293775587322122e3)/12.293775587322122e3

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
if (ratio1<1e-11) and (ratio2<1e-11) and (ratio3<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
