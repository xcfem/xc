# -*- coding: utf-8 -*-
''' Home made test based on the example: https://skyciv.com/docs/tech-notes/loading/asce-7-16-wind-load-calculation-example-for-l-shaped-building/#wind-directionality-factor-k-_-d'''

from __future__ import print_function
from __future__ import division
from actions.wind import base_wind
from actions.wind import ASCE7_wind
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

#                                Roof ridge
#                    /\
#                   /  \
#                  /    \        Mean roof height
#                 /      \
#                /        \
#               -----------      Eave heigth
#               |         |
#               |         |
#               |         |
#               |         |
#               |         |
#           ____________________ Ground
#

V= 52 # Basic wind speed (m/s)
exposureCategory= 'C' # Exposure category
Kd= 0.85 # Wind directionality factor.
Kzt= 1.0 # Topographic factor.
zg= 110 # Ground elevation above sea level.
eaveHeight= 5 # Eave height is the distance from the ground surface
              # adjacent to the building to the roof eave line at
              # a particular wall.
              
meanRoofHeight= 6.5 # the Mean Roof Height (h) is defined as the average
                    # of the roof eave height and the height to the
                    # highest point on the roof surface

Ke= ASCE7_wind.Ke(zg) # Ground elevation factor
KeRef= 0.9869953014457066
ratio1= abs(Ke-KeRef)/KeRef

# Velocity pressure coefficients
eaveHeightKz= ASCE7_wind.Kz(exposure= exposureCategory, z= eaveHeight) 
eaveHeightKzRef= 0.8650282179175907
ratio2= abs(eaveHeightKz-eaveHeightKzRef)/eaveHeightKzRef

meanRoofHeightKz= ASCE7_wind.Kz(exposure= exposureCategory, z= meanRoofHeight)
meanRoofHeightKzRef= 0.9141518634861495
ratio3= abs(meanRoofHeightKz-meanRoofHeightKzRef)/meanRoofHeightKzRef

# Velocity Pressure
eaveHeight_qz= ASCE7_wind.qz(z= eaveHeight, Kz= eaveHeightKz, Kzt= Kzt, Kd= Kd, zg= zg, V= V)
eaveHeight_qz_ref= 1202.9053251380185
ratio4= abs(eaveHeight_qz-eaveHeight_qz_ref)/eaveHeight_qz_ref

meanRoofHeight_qz= ASCE7_wind.qz(z= meanRoofHeight, Kz= meanRoofHeightKz, Kzt= Kzt, Kd= Kd, zg= zg, V= V)
meanRoofHeight_qz_ref= 1271.2165011443503
ratio5= abs(meanRoofHeight_qz-meanRoofHeight_qz_ref)/meanRoofHeight_qz_ref

# Gust effect factor
G= 0.85

# Internal pressure
# The plant structure is assumed to have openings that satisfy the
# definition of a partially enclosed building in Section 26.2 of ASCE 7-16
GCpi= 0.55
pi_plus= GCpi*meanRoofHeight_qz
pi_minus= -pi_plus

# External pressure coefficient.

## Wall external pressure coefficients.
windwardWallCp= ASCE7_wind.wallExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.windward,L= 24, B= 28)

'''
print("Ke= ", Ke)
print("KeRef= ", KeRef)
print("ratio1= ", ratio1)
print("eaveHeightKz= ", eaveHeightKz)
print("eaveHeightKzRef= ", eaveHeightKzRef)
print("ratio2= ", ratio2)
print("meanRoofHeightKz= ", meanRoofHeightKz)
print("meanRoofHeightKzRef= ", meanRoofHeightKzRef)
print("ratio3= ", ratio3)
print("eaveHeight_qz= ", eaveHeight_qz, 'Pa')
print("eaveHeight_qz_ref= ", eaveHeight_qz_ref, 'Pa')
print("ratio4= ", ratio4)
print("meanRoofHeight_qz= ", meanRoofHeight_qz, 'Pa')
print("meanRoofHeight_qz_ref= ", meanRoofHeight_qz_ref, 'Pa')
print("ratio5= ", ratio5)
print("windwardWallCp= ", windwardWallCp)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-6):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')
