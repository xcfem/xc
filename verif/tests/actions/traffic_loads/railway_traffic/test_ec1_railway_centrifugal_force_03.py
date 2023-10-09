# -*- coding: utf-8 -*-
''' Test values of slipstream effect according to EC1-2 clause 6.6. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from actions.railway_traffic import EC1_rail_load_models as ec1
from actions.railway_traffic import track_cross_section_geometry as tcs

V= 200 # Maximum train speed (km/h)
v= V/3.6 # Maximum train speed (m/s)
r= 2200 # radius of curvature.

Lf=  5.0 # influence length of the loaded part of curved track on the bridge, which is most unfavourable for the design of the structural element under consideration [m].

# Track cross-section parameters.
h= 1.8 # height of the train center of gravity with respect to the reference
       # plane.
s= 1.435+80e-3 # distance between rail axes.
u= 0.15 # cant.

cs= tcs.TrackCrossSection(s= s, u= u) 

locomotiveCentrifugalLoads= ec1.locomotiveLM1.getCentrifugalWheelLoads(v= v, Lf= Lf, r= r, trackCrossSection= cs, h= h)

refLeft= geom.Vector2d(23684.479015171197, -45202.092065801895)
refRight= geom.Vector2d(14733.56969521043, -refLeft.y)
refValues= 4*[refLeft]+4*[refRight]

# Check values.
err= 0.0
for load, refValue in zip(locomotiveCentrifugalLoads, refValues):
    err+= (load-refValue).getModulus2()
err= math.sqrt(err)

'''
print(locomotiveCentrifugalLoads)
print(refValues)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
