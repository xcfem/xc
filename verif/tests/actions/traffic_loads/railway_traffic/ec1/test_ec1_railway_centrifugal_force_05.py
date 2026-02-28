# -*- coding: utf-8 -*-
''' Test values of centrifugal load according to clause 6.5.1 of EC1-2 clause 6.6. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.railway_traffic import EC1_rail_load_models as ec1

V= 160 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 1280 # radius of curvature.

Lf=  170.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].

locomotiveLoad= ec1.LocomotiveLoad()

centrifugalLoadPerWheel= locomotiveLoad.getCentrifugalLoadPerWheel(v= v, Lf= Lf, r= r)


# Check values.
err= 0.0
refValue= 14.990817520966214e3
ratio1= abs(centrifugalLoadPerWheel-refValue)/refValue

'''
print('centrifugal load per wheel: ',centrifugalLoadPerWheel/1e3)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
