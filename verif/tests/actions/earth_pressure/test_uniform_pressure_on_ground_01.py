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
import geom
from actions.earth_pressure import earth_pressure as ep

# Define pressure.
qUnif= 5e3
kSoils= [0.25, 0.5] # earth pressure coefficients.
zBottomSoils= [-5,-1e3]
soil= ep.UniformPressureOnBackfill(zGround=0, zBottomSoils= zBottomSoils, KSoils= kSoils, qUnif= qUnif)

# Compute pressure values at some depths.
depths= [0, -4.9, -5, -5.1, -10, -15]
pressures= list()
for z in depths:
    pressures.append(soil.getPressure(z))

# Compute force on a surface.
surf= geom.Segment2d(geom.Pos2d(0,0), geom.Pos2d(0, -10))
forces= soil.getForces2D(surf, 6)

# Reference values.
topSoilForce= qUnif*kSoils[0]*zBottomSoils[0]
bottomSoilForce= qUnif*kSoils[1]*(surf.getToPoint().y-zBottomSoils[0])
forceRefValue= topSoilForce+bottomSoilForce
topSoilForceMoment= -topSoilForce*(zBottomSoils[0]/2.0)
bottomSoilForceMoment= -bottomSoilForce*(zBottomSoils[0]+(surf.getToPoint().y-zBottomSoils[0])/2)
momentRefValue= topSoilForceMoment+bottomSoilForceMoment

# Check values.
err= 0.0
refPressures= [1250]*3+[2500]*3
for pressure, refPressure in zip(pressures, refPressures):
    err+= (pressure-refPressure)**2
err= math.sqrt(err)

ratio1= abs(forces.getResultant().x-forceRefValue)/abs(forceRefValue)
ratio2= abs(forces.getMoment()-momentRefValue)/abs(momentRefValue)

'''
print('err= ', err)
print('forces= ', forces)
print('force reference value: ', forceRefValue)
print('ratio1= ', ratio1)
print('top soil force moment: ', topSoilForceMoment)
print('bottom soil force moment: ', bottomSoilForceMoment)
print('reference moment moment: ', momentRefValue)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-3) and abs(ratio1)<1e-6 and abs(ratio2)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
