# -*- coding: utf-8 -*-
'''Check computation of wind pressures on a duopitch canopy.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.wind import ec1_wind

# Structural factor according to section 6 (see 6.2(1))
c_dir= 1.0
c_season= 1.0

# Compute the value of the probability factor.
T= 100 # year
k= 0.2 # k=0.2 (see IAP-11 4.2.1)
n= 0.5 # n=0.2 (see IAP-11 4.2.1)
c_prob= ec1_wind.get_probability_factor(T= 100.0, K= k, n= n)

# Compute basic velocity
vb0= 26
vb= c_prob*c_dir*c_season*vb0
rho= 1.25

# Compute peak velocity pressure.
terrainCategory= 'III'
c0= 1.0 # orography factor.
rho= 1.25 # air density.
k1= 1.0 # turbulence factor.
reference_height= 3.5
zMax= 200.0 # maximum height according to clause 4.3.2 of EN 1991-1-4:2005.
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= reference_height, zMax= zMax, rho= rho, k1= k1, c0= c0)


# Pressure and force coefficients according to clause 7.3 of EN 1991-1-4:2005.

## Degree of blockage
degree_of_blockage= 1.0

## Overall force coefficients.
alpha1= math.atan2(0.3, 3) # short cantilever.
alpha2= math.atan2(0.5, 5) # long cantilever.
roofAngleRadians= -(alpha1+alpha2)/2.0

maxCf, minCf= ec1_wind.get_duopitch_canopy_overall_force_coefficients(roofAngleRadians= roofAngleRadians, degreeOfBlockage= degree_of_blockage)

## Wind arrangements according to figure 7.17 of 

# Calculating forces from surface pressures EN 1991-1-4:2005.
# each member of the tuple correspond to a side of the
# duopitch canopy (leftSideCF, rightSideCF).
windArrangements= list()
for cf in [maxCf, minCf]:
    windArrangements.extend([(cf,cf), (cf, 0), (0,cf)])

# Compute pressures for each arrangement according to equation (5.3) of
# EN 1991-1-4:2005.
windPressureArrangements= list()
for (cfL, cfR) in windArrangements:
    pL= c_dir*c_season*cfL*qp
    pR= c_dir*c_season*cfR*qp
    windPressureArrangements.append((pL, pR))

windPressureArrangementsRef= [(183.37670598165684, 183.37670598165684), (183.37670598165684, 0.0), (0.0, 183.37670598165684), (-766.9851807755035, -766.9851807755035), (-766.9851807755035, 0.0), (0.0, -766.9851807755035)]

err= 0.0
for wpArrangement, wpArrangementRef in zip(windPressureArrangements, windPressureArrangementsRef):
    for i in [0, 1]:
        err+= (wpArrangement[i]-wpArrangementRef[i])**2
err= math.sqrt(err)

'''
from postprocess.reports import common_formats as cfmt
print('Probability factor _prob(T='+str(T)+', k= '+str(k)+', n= '+str(n)+')= '+ cfmt.Factor.format(c_prob))
print('Air density: ', rho)
print('Peak velocity pressure: '+cfmt.Pressure.format(qp/1e3)+' kN/m2')
print('Overall force coefficients:')
print('  roof angle: '+ cfmt.Angle.format(math.degrees(roofAngleRadians))+'º')
print('  Maximum: '+ cfmt.Factor.format(maxCf))
print('  Minimum: '+ cfmt.Factor.format(minCf))
print('Wind pressure arrangements:')
for i, wpArrangement in enumerate(windPressureArrangements):
    print('  arrangement: ', i)
    print('    pressure on left side:',cfmt.Pressure.format( wpArrangement[0]/1e3), 'kN/m2')
    print('    pressure on right side:', cfmt.Pressure.format(wpArrangement[1]/1e3), 'kN/m2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
