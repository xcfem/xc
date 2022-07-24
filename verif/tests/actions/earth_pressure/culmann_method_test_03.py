# -*- coding: utf-8 -*-
''' Pantalla de pilotes. Relación entre el coeficiente de empuje calculado
según Rankine, Culmann y Coulomb.'''

from __future__ import division
from __future__ import print_function

import sys
import math
import geom
from geotechnics import frictional_soil
from geotechnics import earth_pressure

# Wall back segment.
ptA= geom.Pos2d(0,0)
ptB= geom.Pos2d(-1.938e-09,-24.6724)
wallBack= geom.Segment2d(ptA, ptB)
wallHeight= wallBack.getLength()

# Backfill profile.
backfillProfilePoints= [geom.Pos2d(0, 0), geom.Pos2d(0.642, 0.327551), geom.Pos2d(2.561, 0.826551), geom.Pos2d(2.566, 0.790551), geom.Pos2d(3.573, 1.32755), geom.Pos2d(3.888, 1.39955), geom.Pos2d(4.328, 1.55755), geom.Pos2d(6.106, 1.55755), geom.Pos2d(6.886, 1.44655), geom.Pos2d(7.529, 1.32755), geom.Pos2d(8.259, 0.837551), geom.Pos2d(8.636, 0.826551), geom.Pos2d(10.352, 0.327551), geom.Pos2d(11.174, -0.172449), geom.Pos2d(12.507, -0.672449), geom.Pos2d(13.683, -1.17245), geom.Pos2d(14.657, -1.67245), geom.Pos2d(15.774, -2.17245), geom.Pos2d(16.756, -2.67245), geom.Pos2d(17.456, -3.17345), geom.Pos2d(18.239, -3.67245), geom.Pos2d(19.126, -4.17245), geom.Pos2d(20.32, -4.67245), geom.Pos2d(21.348, -5.17245), geom.Pos2d(22.658, -5.67245), geom.Pos2d(24.061, -6.17245), geom.Pos2d(25.096, -6.32045), geom.Pos2d(27.272, -6.44845), geom.Pos2d(29.266, -6.67245), geom.Pos2d(30.993, -7.04645), geom.Pos2d(33.473, -7.17345), geom.Pos2d(53.1, -7.29045), geom.Pos2d(1000, -7.29045)]
backfillProfile=geom.Polyline2d(backfillProfilePoints)

# Backfill slope angle.
crestPt= backfillProfile[0]
crestZ= crestPt.y
for pt in backfillProfile[1:]:
    if(pt.y>crestZ):
        crestPt= pt
        crestZ= crestPt.y
firstSegment= geom.Segment2d(ptA, crestPt)
backfillSlopeAngle= firstSegment.getAngle(geom.Vector2d(1,0))

# Soil
soil= frictional_soil.FrictionalSoil(phi= math.radians(30), rho= 2.0e3)
delta= math.radians(10) # friction angle of the soil and the wall.

# Culmann method.
maxPressureCulmann, pressureFunction, minWeight, maxWeight= earth_pressure.active_pressure_culmann_method(soil= soil, wallBack= wallBack, backfillProfile= backfillProfile, delta= delta)
KaCulmann= maxPressureCulmann/(0.5*soil.gamma()*wallHeight**2)
# # Coulomb method.
# KaCoulomb= earth_pressure.ka_coulomb(a= 0.0, b= backfillSlopeAngle, fi= soil.phi, d= delta)
# maxPressureCoulomb= 0.5*soil.gamma()*wallHeight**2*KaCoulomb
# # Rankine method.
# KaRankine= earth_pressure.ka_rankine(b= backfillSlopeAngle, fi= soil.phi)
# maxPressureRankine= 0.5*soil.gamma()*wallHeight**2*KaRankine

ratio= abs(maxPressureCulmann-1894.211278183068e3)/1894.211278183068e3

'''
print('active earth pressure coefficient (Culmann Method): ', KaCulmann)
print('max pressure (Culmann Method): ', maxPressureCulmann/1e3, 'kN/m')
print('active earth pressure coefficient (Coulomb Method): ', KaCoulomb)
print('max pressure (Coulomb Method): ', maxPressureCoulomb/1e3, 'kN/m')
print('active earth pressure coefficient (Rankine Method): ', KaRankine)
print('max pressure (Rankine Method): ', maxPressureRankine/1e3, 'kN/m')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<.1:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
