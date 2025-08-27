# -*- coding: utf-8 -*-
'''Check computation of wind pressures on a monopitch canopy.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from actions.wind import ec1_wind
from geom_utils import parabola as pb

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
roofAngle= math.radians(20)
leftBorder= geom.Pos2d(0,0)
rightBorder= geom.Pos2d(5.0, -math.sin(roofAngle))
canopySegment= geom.Segment2d(leftBorder, rightBorder)
normalVector= canopySegment.getNormal()

maxCf, minCf= ec1_wind.get_monopitch_canopy_overall_force_coefficients(roofAngleRadians= roofAngle, degreeOfBlockage= degree_of_blockage)

windDirections= [1, -1]
# Make sure the normal vector points upwards.
if(normalVector.y<0):
    normalVector= -normalVector
# Compute pressures for each arrangement according to equation (5.3) of
# EN 1991-1-4:2005.
# For each case store the pressures in a tuple:
# (leftBorderPressure, rightBorderPressure)
pressures= list()
for windDir in windDirections:
    for cf in [maxCf, minCf]:
        maxWindPressure=  c_dir*c_season*cf*qp
        if(windDir>0): # wind from the left.
            vertex= rightBorder
            pt= leftBorder+normalVector*maxWindPressure
            windwardPt= leftBorder
        else:
            vertex= leftBorder
            pt= rightBorder+normalVector*maxWindPressure
            windwardPt= rightBorder
        ref= geom.Ref2d2d(vertex,geom.Vector2d(-normalVector.y, normalVector.x))
        pressureParabola= pb.define_generalized_parabola_from_vertex_and_point(ref= ref, vertex= vertex, pt= pt)
        pressureAtVertex= ref.getLocalPosition(pressureParabola.y(vertex)).y
        pressureAtWindwardPt= ref.getLocalPosition(pressureParabola.y(windwardPt)).y
        if(windDir>0):
            pressures.append((pressureAtWindwardPt, pressureAtVertex))
        else:
            pressures.append((pressureAtVertex, pressureAtWindwardPt))

refPressures= [(-2339.8890631219647, 0.0), (4094.8058604633743, 0.0), (0.0, -2339.889063121968), (0.0, 4094.805860463413)]

err= 0.0
for pressure, refPressure in zip(pressures, refPressures):
    for i in [0, 1]:
        err+= (pressure[i]-refPressure[i])**2
err= math.sqrt(err)


'''
from postprocess.reports import common_formats as cfmt
print('Probability factor _prob(T='+str(T)+', k= '+str(k)+', n= '+str(n)+')= '+ cfmt.Factor.format(c_prob))
print('Basic wind velocity vb= ', cfmt.Speed.format(vb), 'm/s')
print('Air density: ', rho)
print('Peak velocity pressure: '+cfmt.Pressure.format(qp/1e3)+' kN/m2')
print('Overall force coefficients:')
print('  roof angle: '+ cfmt.Angle.format(math.degrees(roofAngle))+'º')
print('  Maximum: '+ cfmt.Factor.format(maxCf))
print('  Minimum: '+ cfmt.Factor.format(minCf))
print('Wind pressure arrangements:')
for i, pressure in enumerate(pressures):
    print('  arrangement: ', i)
    print('    pressure on left border:', cfmt.Pressure.format(pressure[0]/1e3), 'kN/m2')
    print('    pressure on right border:', cfmt.Pressure.format(pressure[1]/1e3), 'kN/m2')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
