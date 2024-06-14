# -*- coding: utf-8 -*-
''' Test values of traction and braking forces according to EC1-2 clause 
6.5.3 paragraph 2. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.railway_traffic import EC1_rail_load_models

Lab= 7.98 # Effective length.

# Compute braking and traction load.
brakingLoad= EC1_rail_load_models.get_braking_force(Lab= Lab, loadModel= '71')
tractionLoad= EC1_rail_load_models.get_traction_force(Lab= Lab)


# Check the results.
# Values from "Design of Reinforced Concrete Railway Bridges with Spans from 3 to 9 meters" MASTER'S THESIS Adolfo Martinez Diaz 2014. Luleå University of Technology. Expressions (4.21) and (4.22) page 101.
ratio1= abs(brakingLoad-159.6e3)/159.6e3
ratio2= abs(tractionLoad-263.34e3)/263.34e3

'''
print(brakingLoad/1e3, 'kN', ratio1)
print(tractionLoad/1e3, 'kN', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) and (ratio2<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


