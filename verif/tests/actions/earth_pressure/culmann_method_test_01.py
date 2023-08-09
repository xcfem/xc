# -*- coding: utf-8 -*-
''' Culmann method verification test. Inspired on the Problem 7.2
    from Athanasopoulos, G. A. (2021) 
    "Online Lecture Notes on Soil Mechanics", Geoengineer.org, 
    DOI: https://doi.org/10.48246/GEOENG-EDU-001
'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import math
import geom
from geotechnics import frictional_soil
from geotechnics import earth_pressure

# Wall
wallHeight= 6.0

# Soil
soil= frictional_soil.FrictionalSoil(phi= math.radians(38), rho= 1.8e3)
delta= math.radians(20) # friction angle of the soil and the wall.

# Wall back segment.
ptA= geom.Pos2d(0.0,0.0)
ptB= geom.Pos2d(0.0,-6.0)
wallBack= geom.Segment2d(ptA, ptB)

# Backfill profile.
backfillSlopeAngle= math.radians(20)
backfillProfile= geom.Polyline2d([ptA, ptA+1e3*geom.Vector2d(math.cos(backfillSlopeAngle), math.sin(backfillSlopeAngle))])

# Compute active pressure coefficient.
## According to Culmann method.
maxPressureCulmann, pressureFunction, minWeight, maxWeight= earth_pressure.active_pressure_culmann_method(soil= soil, wallBack= wallBack, backfillProfile= backfillProfile, delta= delta)
KaCulmann= maxPressureCulmann/(0.5*soil.gamma()*wallHeight**2)

## According to Coulomb method.
KaCoulomb= earth_pressure.ka_coulomb(a= 0.0, b= backfillSlopeAngle, phi=  soil.phi, d= delta)
maxPressureCoulomb= 0.5*soil.gamma()*wallHeight**2*KaCoulomb

ratio= abs(maxPressureCulmann-maxPressureCoulomb)/maxPressureCoulomb
'''
print('active earth pressure coefficient (Culmann Method): ', KaCulmann)
print('max pressure (Culmann Method): ', maxPressureCulmann/1e3, 'kN/m')
print('active earth pressure coefficient (Coulomb Method): ', KaCoulomb)
print('max pressure (Coulomb Method): ', maxPressureCoulomb/1e3, 'kN/m')
print('ratio: ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<.1:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
# import matplotlib.pyplot as plt
# xnew= list()
# numSteps= 20
# step= (maxWeight-minWeight)/numSteps
# x= minWeight
# for i in range(0, numSteps+1):
#     xnew.append(x)
#     x+= step
    
# ynew = pressureFunction(xnew)
# plt.plot(xnew, ynew, '-')

# plt.show()
