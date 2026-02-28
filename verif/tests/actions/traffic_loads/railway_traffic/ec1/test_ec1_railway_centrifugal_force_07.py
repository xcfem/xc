# -*- coding: utf-8 -*-
''' Test values of centrifugal load according to clause 6.5.1 of EC1-2 clause 6.6. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import EC1_rail_load_models


V= 250 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)

Lf=  63.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural elenlent under consideration [m].

# Compute reduction factor of the centrifugal force according to
# expression 6.19 of Eurocode 1 part 2 (clause 6.5.1 paragraph 8).
f= EC1_rail_load_models.centrifugal_force_reduction_factor(v= v, Lf= Lf)

classificationFactor= 1.21
dynamicFactor= 1.06

r= 3550 # radius of curvature.

# Compute centrifugal forces using the train load model.
locomotiveLM1= EC1_rail_load_models.get_locomotive_LM1()
lm71= EC1_rail_load_models.TrainLoadModel(locomotive= locomotiveLM1, uniformLoad= 80e3, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
(Qtk1, qtk1)= lm71.getCentrifugalForces(v= v, Lf= Lf, r= r)

# Compute centrifugal forces using the get_centrifugal_forces function.
# characteristic value of the vertical concentrated load 
# specified in clause 6.3 (alpha= 1.21)
Qvk= 250e3*dynamicFactor # per axis (3 axis on the bridge)

# characteristic value of the vertical distributed load 
# specified in clause 6.3 (alpha= 1.21)
qvk= 80e3*dynamicFactor # per meter
(Qtk2, qtk2)= EC1_rail_load_models.get_centrifugal_forces(v= v, Lf= Lf, r= r, Qvk= Qvk, qvk= qvk, alpha= classificationFactor)

# Check results.
qtk_ratio= abs(qtk2-qtk1)/qtk1
Qtk_ratio= abs(Qtk2-Qtk1)/Qtk1

'''
print('Reduction factor of the centrifugal force: f= ', f)
print('qtk1= ', qtk1/1e3, 'kN/m')
print('qtk2= ', qtk2/1e3, 'kN/m')
print('qtk_ratio= ', qtk_ratio)
print('Qtk1= ', Qtk1/1e3, 'kN/axis')
print('Qtk2= ', Qtk2/1e3, 'kN/axis')
print('Qtk_ratio= ', Qtk_ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(qtk_ratio<1e-6 and Qtk_ratio<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


