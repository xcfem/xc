# -*- coding: utf-8 -*-
''' Compute the wind force on a bridge deck with traffic.

Based on the example 3.3.1 of the publication Bridge Design to Eurocodes
Worked examples. European Commission 2012. ISBN 978-92-79-22823-0
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
from actions.wind import ec1_wind

# Compute the value of the probability factor.
c_prob= ec1_wind.get_probability_factor(T= 100)
ratio1= abs(c_prob-1.0384765480213738)/1.0384765480213738

c_dir= 1.0
c_season= 1.0
# Compute basic velocity
vb0= 26
vb= c_prob*c_dir*c_season*vb0
rho= 1.25
# Compute basic velocity pressure
qb= ec1_wind.get_basic_velocity_pressure(vb= vb, rho= rho)
ratio2= abs(qb-455.63)/455.63
# Compute roughness factor
terrainCategory= 'II'
z= 10.0 # Squat pier
zMax= 200.0
cr= ec1_wind.get_roughness_factor(terrainCategory= terrainCategory, z= z, zMax= zMax)
ratio3= abs(cr-1.0066802996441269)/1.0066802996441269
# Compute turbulence intensity
c0= 1.0 # orography factor
vm= cr*c0*vb
k1= 1.0 # turbulence factor.
Iv= ec1_wind.get_turbulence_intensity(terrainCategory= terrainCategory, z= z, zMax= zMax, k1= k1, c0= c0)
ratio4= abs(Iv-0.18873916581775485)/0.18873916581775485

# Compute peak velocity pressure.
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
qpRef= (1+7*Iv)*1/2.0*rho*vm**2 # There is an error in the example.
ratio5= abs(qp-qpRef)/qpRef

# Compute exposure factor.
ce= ec1_wind.get_exposure_factor(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
ratio6= abs(ce-2.352290024218277)/2.352290024218277

# Depth subjected to wind pressure (see figure 1.11 of the book).
girdersDepth= 2.8
deckThickness= 0.4+0.025*3.5
asphaltLayerThickness= 0.08
trafficDepth= 2.0
dtot= girdersDepth+deckThickness+asphaltLayerThickness+trafficDepth
ratio7= abs(dtot-5.3675)/5.3675

# Compute depth ratio
deckWidth= 12.0
depthRatio= deckWidth/dtot

# Compute force coefficient.
cfx0= ec1_wind.get_bridge_deck_transverse_force_coefficient(b= deckWidth, dtot= dtot, solidParapets= True)
ratio8= abs(cfx0-1.8345483868372523)/1.8345483868372523
cfx= cfx0 # Equation (8.1) of EN 1991-1-4:2005.

# Compute wind pressure on the bridge deck.
cscd= 1.0 # structural factor.
pw= cscd*cfx*qp

# Compute reference area
deckLength= 200.0
Aref= dtot*deckLength

# Compute transverse wind force
Fw= pw*Aref
ratio9= abs(Fw-2110.7761886460303e3)/2110.7761886460303e3

# Compute vertical wind force.
# Compute vertical pressure distribution.
## Here d_tot must be computed disregarding the traffic
## and any bridge equipment
concreteSupport= 0.2
dtotVP= girdersDepth+deckThickness+concreteSupport
alph= math.radians(10)
beta= 0.0
cfz= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= deckWidth, dtotVP= dtotVP, alpha= alph, beta= beta)
Fwz= cscd*cfx*qp*deckWidth*deckLength
vpd= ec1_wind.get_vertical_pressure_distribution(terrainCategory= terrainCategory, x0= -deckWidth/2.0, x1= deckWidth/2.0, dtotVP= dtotVP, z= z, vb= vb, zMax= zMax, rho= rho, k1= k1, c0= c0, alpha= alph, beta= beta)

ratio10= abs(cfz-0.8790322580645161)/0.8790322580645161

'''
print('c_prob= ', c_prob, ratio1)
print('vb= ', vb)
print('vm= ', vm)
print('qb= ', qb, ratio2)
print('cr= ', cr, ratio3)
print('Iv= ', Iv, ratio4)
print('qp= ', qp, ratio5)
print('ce= ', ce, ratio6)
print('dtot= ', dtot, ratio7)
print('depthRatio= ', depthRatio)
print('cfx0= ', cfx0, ratio8)
print('pw= ', pw/1e3,'kN/m2')
print('Aref= ', Aref,'m2')
print('Fw= ', Fw/1e3,'kN', ratio9)
print('cfz= ', cfz, ratio10)
print('Fwz= ', Fwz/1e3,'kN', Fwz/deckLength/1e3, 'kN/m')
vp0= (float(vpd[0](-deckWidth/2.0)), float(vpd[0](deckWidth/2.0)))
vp1= (float(vpd[1](-deckWidth/2.0)), float(vpd[1](deckWidth/2.0)))
print(vp0)
print(vp1)
print(cfz, ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4) and (abs(ratio2)<1e-4) and (abs(ratio3)<1e-4) and (abs(ratio4)<1e-4) and (abs(ratio5)<1e-4) and (abs(ratio6)<1e-4) and (abs(ratio7)<1e-4) and (abs(ratio8)<1e-4)and (abs(ratio9)<1e-4) and (abs(ratio10)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
