# -*- coding: utf-8 -*-
''' Home made test based on the example: https://skyciv.com/docs/tech-notes/loading/asce-7-16-wind-load-calculation-example-for-l-shaped-building/#wind-directionality-factor-k-_-d'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

from actions.wind import base_wind
from actions.wind import ASCE7_wind
import math

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
L= 24 # Horizontal dimension of building, measured parallel to wind direction.
B= 28 # Horizontal dimension of building, measured normal to wind direction.
## Wall external pressure coefficients.
windwardWallCp= ASCE7_wind.wallExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.windward,L= L, B= 28)
windwardWallCpRef= 0.8
ratio6= abs(windwardWallCp-windwardWallCpRef)/windwardWallCpRef
leewardWallCp= ASCE7_wind.wallExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.leeward,L= L, B= 28)
leewardWallCpRef= -0.5
ratio7= abs(leewardWallCp-leewardWallCpRef)/-leewardWallCpRef
sideWallsCp= ASCE7_wind.wallExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.side,L= L, B= 28)
sideWallsCpRef= -0.7
ratio8= abs(sideWallsCp-sideWallsCpRef)/-sideWallsCpRef

## Roof pressure coefficients.
tht= 36.87 # Angle of plane of roof from horizontal, in degrees.

### Wind normal to ridge.
windwardRoofCp= ASCE7_wind.roofExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.windward, L= L, h= meanRoofHeight, theta= tht)[1]
windwardRoofCpRef= 0.39322500000000005
ratio12= abs(windwardRoofCp-windwardRoofCpRef)/windwardRoofCpRef
leewardRoofCp= ASCE7_wind.roofExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.leeward, L= L, h= meanRoofHeight, theta= tht)[0]
leewardRoofCpRef= -0.6
ratio13= abs(leewardRoofCp-leewardRoofCpRef)/-leewardRoofCpRef
### Wind parallel to ridge
pllRoofCp05h= ASCE7_wind.roofExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.side, L= L, h= meanRoofHeight, theta= tht, hDistFromWindwardEdge= 0.5*meanRoofHeight)
pllRoofCp05hRef= [-0.9, -0.18]
ratio14= math.sqrt((abs(pllRoofCp05h[0]-pllRoofCp05hRef[0])/-pllRoofCp05hRef[1])**2+(abs(pllRoofCp05h[1]-pllRoofCp05hRef[1])/-pllRoofCp05hRef[1])**2)
pllRoofCp1_5h= ASCE7_wind.roofExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.side, L= L, h= meanRoofHeight, theta= tht, hDistFromWindwardEdge= 1.5*meanRoofHeight)
pllRoofCp1_5hRef= [-0.5, -0.18]
ratio15= math.sqrt((abs(pllRoofCp1_5h[0]-pllRoofCp1_5hRef[0])/-pllRoofCp1_5hRef[1])**2+(abs(pllRoofCp1_5h[1]-pllRoofCp1_5hRef[1])/-pllRoofCp1_5hRef[1])**2)
pllRoofCp3h= ASCE7_wind.roofExternalPressureCoefficient(orientation= base_wind.windSurfaceOrientation.side, L= L, h= meanRoofHeight, theta= tht, hDistFromWindwardEdge= 3.0*meanRoofHeight)
pllRoofCp3hRef= [-0.3, -0.18]
ratio16= math.sqrt((abs(pllRoofCp3h[0]-pllRoofCp3hRef[0])/-pllRoofCp3hRef[1])**2+(abs(pllRoofCp3h[1]-pllRoofCp3hRef[1])/-pllRoofCp3hRef[1])**2)

## Wall external pressure.
windwardWallPe= windwardWallCp*G*eaveHeight_qz
windwardWallPeRef= 817.9756210938526
ratio9= abs(windwardWallPe-windwardWallPeRef)/windwardWallPeRef
leewardWallPe= leewardWallCp*G*meanRoofHeight_qz
leewardWallPeRef= -540.2670129863488
ratio10= abs(leewardWallPe-leewardWallPeRef)/-leewardWallPeRef
sideWallsPe= sideWallsCp*G*meanRoofHeight_qz
sideWallsPeRef= -756.3738181808884
ratio11= abs(sideWallsPe-sideWallsPeRef)/-sideWallsPeRef

## Roof external pressure.
### Wind normal to ridge.
windwardRoofPe= windwardRoofCp*G*meanRoofHeight_qz
windwardRoofPeRef= 424.89299236311416
ratio17= abs(windwardRoofPe-windwardRoofPeRef)/windwardRoofPeRef
leewardRoofPe= leewardRoofCp*G*meanRoofHeight_qz
leewardRoofPeRef= -648.3204155836187
ratio18= abs(leewardRoofPe-leewardRoofPeRef)/-leewardRoofPeRef
pllRoofPe05h= [pllRoofCp05h[0]*G*meanRoofHeight_qz, pllRoofCp05h[1]*G*meanRoofHeight_qz]
pllRoofPe05hRef= [-972.480623375428, -194.4961246750856]
ratio19= math.sqrt((abs(pllRoofPe05h[0]-pllRoofPe05hRef[0])/-pllRoofPe05hRef[1])**2+(abs(pllRoofPe05h[1]-pllRoofPe05hRef[1])/-pllRoofPe05hRef[1])**2)
pllRoofPe1_5h= [pllRoofCp1_5h[0]*G*meanRoofHeight_qz, pllRoofCp1_5h[1]*G*meanRoofHeight_qz]
pllRoofPe1_5hRef= [-540.2670129863488, -194.4961246750856]
ratio20= math.sqrt((abs(pllRoofPe1_5h[0]-pllRoofPe1_5hRef[0])/-pllRoofPe1_5hRef[1])**2+(abs(pllRoofPe1_5h[1]-pllRoofPe1_5hRef[1])/-pllRoofPe1_5hRef[1])**2)
pllRoofPe3h= [pllRoofCp3h[0]*G*meanRoofHeight_qz, pllRoofCp3h[1]*G*meanRoofHeight_qz]
pllRoofPe3hRef= [-324.16020779180934, -194.4961246750856]
ratio21= math.sqrt((abs(pllRoofPe3h[0]-pllRoofPe3hRef[0])/-pllRoofPe3hRef[1])**2+(abs(pllRoofPe3h[1]-pllRoofPe3hRef[1])/-pllRoofPe3hRef[1])**2)

# Critical load condition
# Values of external and internal pressures shall be
# combined algebraically to determine the most critical load.

externalPressures= [['windward wall', [windwardWallPe] ],
                    ['leeward wall', [leewardWallPe] ],
                    ['side wall', [sideWallsPe] ],
                    ['windward roof', [windwardRoofPe] ],
                    ['leeward roof', [leewardRoofPe] ],
                    ['flat (along ridge) 0 to h', pllRoofPe05h ],
                    ['flat (along ridge) h to 2h', pllRoofPe1_5h ],
                    ['flat (along ridge) >2h', pllRoofPe3h ]]

combinedPressures= list()
for row in externalPressures:
    surface= row[0]
    pressureRow= list()
    for p in row[1]:
        top= p+pi_plus # add internal pressure.
        bottom= p+pi_minus
        pressureRow.append(top)
        pressureRow.append(bottom)
    combinedPressures.append([surface, pressureRow])

combinedPressuresRef= [['windward wall', [1517.1446967232455, 118.80654546445987]], ['leeward wall', [158.9020626430439, -1239.4360886157415]], ['side wall', [-57.204742551495656, -1455.5428938102812]], ['windward roof', [1124.0620679925069, -274.2760832662786]], ['leeward roof', [50.84866004577407, -1347.4894912130114]], ['flat (along ridge) 0 to h', [-273.3115477460352, -1671.6496990048208, 504.6729509543071, -893.6652003044784]], ['flat (along ridge) h to 2h', [158.9020626430439, -1239.4360886157415, 504.6729509543071, -893.6652003044784]], ['flat (along ridge) >2h', [375.0088678375834, -1023.329283421202, 504.6729509543071, -893.6652003044784]]]

ratio22= 0.0
for r1,r2 in zip(combinedPressures, combinedPressuresRef):
    for p1, p2 in zip(r1[1], r2[1]):
      ratio22+= (p1-p2)**2
ratio22= math.sqrt(ratio22)

# # Uncomment to export results into CSV file
# import csv
# with open('pressures.csv', mode='w') as pressures:
#     writer = csv.writer(pressures)
#     for r in combinedPressures:
#         row= [r[0]]
#         for p in r[1]:
#           row.append(p)
#         writer.writerow(row)
# check topographic multipliers
def err(lstcomp,lstres):
    lstdif=[(lstcomp[i]-lstres[i])**2 for i in range(len(lstcomp))]
    return math.sqrt(sum(lstdif))
    
Lh=1
H=0.3
x=2
z=0.6
# 2D ridge
k123_2DR=ASCE7_wind.K1K2K3('2DR',H,Lh,x,False,z)
k123_2DR_comp=0.43*0*0.17
ratio23=abs(k123_2DR-k123_2DR_comp)
# 2D escarpment
k123_2DE=ASCE7_wind.K1K2K3('2DE',H,Lh,x,False,z)
k123_2DE_comp=0.26*0.5*0.22
ratio24=abs((k123_2DE-k123_2DE_comp)/k123_2DE_comp)
#3D axisymmetrical hill
k123_3DA=ASCE7_wind.K1K2K3('3DA',H,Lh,x,False,z)
k123_3DA_comp=0.32*0.0*0.22
ratio25=abs(k123_3DA-k123_3DA_comp)



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
print("windwardWallCpRef= ", windwardWallCpRef)
print("ratio6= ", ratio6)
print("leewardWallCp= ", leewardWallCp)
print("leewardWallCpRef= ", leewardWallCpRef)
print("ratio7= ", ratio7)
print("sideWallsCp= ", sideWallsCp)
print("sideWallsCpRef= ", sideWallsCpRef)
print("ratio8= ", ratio8)
print("windwardWallPe= ", windwardWallPe, 'Pa')
print("windwardWallPeRef= ", windwardWallPeRef, 'Pa')
print("ratio9= ", ratio9)
print("leewardWallPe= ", leewardWallPe, 'Pa')
print("leewardWallPeRef= ", leewardWallPeRef, 'Pa')
print("ratio10= ", ratio10)
print("sideWallsPe= ", sideWallsPe, 'Pa')
print("sideWallsPeRef= ", sideWallsPeRef, 'Pa')
print("ratio11= ", ratio11)
print("windwardRoofCp= ", windwardRoofCp)
print("windwardRoofCpRef= ", windwardRoofCpRef)
print("ratio12= ", ratio12)
print("leewardRoofCp= ", leewardRoofCp)
print("leewardRoofCpRef= ", leewardRoofCpRef)
print("ratio13= ", ratio13)
print("pllRoofCp05h= ", pllRoofCp05h)
print("pllRoofCp05hRef= ", pllRoofCp05hRef)
print("ratio14= ", ratio14)
print("pllRoofCp1_5h= ", pllRoofCp1_5h)
print("pllRoofCp1_5hRef= ", pllRoofCp1_5hRef)
print("ratio15= ", ratio15)
print("pllRoofCp3h= ", pllRoofCp3h)
print("pllRoofCp3hRef= ", pllRoofCp3hRef)
print("ratio16= ", ratio16)
print("windwardRoofPe= ", windwardRoofPe, 'Pa')
print("windwardRoofPeRef= ", windwardRoofPeRef, 'Pa')
print("ratio17= ", ratio17)
print("leewardRoofPe= ", leewardRoofPe, 'Pa')
print("leewardRoofPeRef= ", leewardRoofPeRef, 'Pa')
print("ratio18= ", ratio18)
print("pllRoofPe05h= ", pllRoofPe05h)
print("pllRoofPe05hRef= ", pllRoofPe05hRef)
print("ratio19= ", ratio19)
print("pllRoofPe1_5h= ", pllRoofPe1_5h)
print("pllRoofPe1_5hRef= ", pllRoofPe1_5hRef)
print("ratio20= ", ratio20)
print("pllRoofPe3h= ", pllRoofPe3h)
print("pllRoofPe3hRef= ", pllRoofPe3hRef)
print("ratio21= ", ratio21)
print("external pressures= ", externalPressures)
print("combined pressures= ", combinedPressures)
print("ratio22= ", ratio22)
print("ratio23= ", ratio23)
print("ratio24= ", ratio24)
print("ratio25= ", ratio25)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6 and ratio5<1e-6 and ratio6<1e-6 and ratio7<1e-6 and ratio8<1e-6 and ratio9<1e-6 and ratio10<1e-6 and ratio11<1e-6and ratio12<1e-6 and ratio13<1e-6 and ratio14<1e-6 and ratio15<1e-6 and ratio16<1e-6 and ratio17<1e-6 and ratio18<1e-6 and ratio19<1e-6 and ratio20<1e-6 and ratio21<1e-6 and ratio22<1e-6 and ratio23<1e-6 and ratio24<1.5e-2  and ratio23<1e-6):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
