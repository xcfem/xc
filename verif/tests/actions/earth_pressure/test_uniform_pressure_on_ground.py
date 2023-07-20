# -*- coding: utf-8 -*-
'''Trivial test of earth pressure due to a uniform load in the backfill
surface.
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import math
from actions.earth_pressure import earth_pressure as ep


soil= ep.UniformPressureOnBackfill(zGround=0, zBottomSoils=[-5,-1e3],KSoils=[0.25, 0.5], qUnif= 5e3, xcSet= None, vDir= None)
depths= [0, -4.9, -5, -5.1, -10, -15]
pressures= list()
for z in depths:
    pressures.append(soil.getPressure(z))

# Check values.
err= 0.0
refPressures= [1250]*2+[2500]*4
for pressure, refPressure in zip(pressures, refPressures):
    err+= (pressure-refPressure)**2
err= math.sqrt(err)

'''
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
