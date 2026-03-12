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

V= 200 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 2200 # radius of curvature.

Lf=  5.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].

locomotiveLM1= ec1.get_locomotive_LM1()
locomotiveCentrifugalLoads= locomotiveLM1.getCentrifugalWheelLoads(v= v, Lf= Lf, r= r)


# Check values.
err= 0.0
refValue= 15.87522673982712e3
for load in locomotiveCentrifugalLoads:
    err+= (load-refValue)**2
err= math.sqrt(err)

'''
print('locomotive centrifugal loads: ', locomotiveCentrifugalLoads)
print('reference value: ', refValue/1e3)
print('error: ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
