# -*- coding: utf-8 -*-
'''Compute positions of the LM1 tandem load on the notional lane.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from actions.roadway_traffic import load_model_base as lmb
from actions.roadway_traffic import EC1_load_models as EC1lm
    

notionalLane= lmb.NotionalLane(geom.Polygon3d([geom.Pos3d(0,2,0), geom.Pos3d(0,5,0), geom.Pos3d(10,5,0), geom.Pos3d(10,2,0)]))

# center= notionalLane.getCentroid()
axis= notionalLane.getAxis()
ref= axis.getReferenceAt(0.5)
# Tandem 300 kN/axle
wheelLoads= axis.getWheelLoads(loadModel= EC1lm.tandem300LM1, lmbdArcLength=0.1, loadFactor= 1.0)

# Reference values.
refPositions= [geom.Pos3d(1.6,2.5,0), geom.Pos3d(0.4,2.5,0), geom.Pos3d(1.6,4.5,0), geom.Pos3d(0.4,4.5,0)]

#print('center: ', center)
#print('axis: ', axis)
#print('reference: ', ref)

err= 0.0
for wl,refp in zip(wheelLoads, refPositions):
    err+= (wl.position-refp).getModulus()**2
    
# print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
