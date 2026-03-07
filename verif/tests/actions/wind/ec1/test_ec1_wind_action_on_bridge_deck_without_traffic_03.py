# -*- coding: utf-8 -*-
''' Compute the wind force on a bridge deck with traffic.

Checked against the calculation in the project named:
PROYECTO DE CONSTRUCCIÓN DE PLATAFORMA DE LA LÍNEA DE ALTA VELOCIDAD DE LA 
CONEXIÓN MADRID – PAÍS VASCO – FRONTERA FRANCESA. TRAMO: BURGOS – VITORIA. 
SUBTRAMO: VARIANTE FERROVIARIA DE BURGOS - VALLE DE LAS NAVAS.

Anejo Nº11. Apéndice 1: Viaducto sobre el Río Vena.
Page 24 of the PDF Document. Transverse wind in deck.
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

# Depth subjected to wind pressure
deckDepth= 3.1
barrierDepth= 1.4
trafficDepth= 4.0
totalDepth= deckDepth+barrierDepth

# Compute the value of the probability factor.
c_prob= ec1_wind.get_probability_factor(T= 100)

c_dir= 1.0
c_season= 1.0
# Compute basic velocity
vb0= 28 # Burgos
vb= c_prob*c_dir*c_season*vb0
rho= 1.25
# Compute basic velocity pressure
qb= ec1_wind.get_basic_velocity_pressure(vb= vb, rho= rho)
# Compute roughness factor
terrainCategory= 'II'
z= 6.74+totalDepth/2.0 # Height above the terrain.
zMax= 200.0
cr= ec1_wind.get_roughness_factor(terrainCategory= terrainCategory, z= z, zMax= zMax)

# Compute turbulence intensity
c0= 1.0 # orography factor
vm= cr*c0*vb
k1= 1.0 # turbulence factor.
Iv= ec1_wind.get_turbulence_intensity(terrainCategory= terrainCategory, z= z, zMax= zMax, k1= k1, c0= c0)

# Compute peak velocity pressure.
qp= ec1_wind.get_peak_velocity_pressure(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)
qpRef= (1+7*Iv)*1/2.0*rho*vm**2 # There is an error in the example.


# Compute exposure factor.
ce= ec1_wind.get_exposure_factor(terrainCategory= terrainCategory, vb= vb, z= z, zMax= zMax, rho= rho, k1= k1, c0= c0)

# Compute depth ratio
deckWidth= 14.0
depthRatio= deckWidth/totalDepth

# Compute force coefficient.
cfx0= ec1_wind.get_bridge_deck_transverse_force_coefficient(b= deckWidth, dtot= totalDepth, solidParapets= True)
cfx= cfx0 # Equation (8.1) of EN 1991-1-4:2005.

# Compute wind pressure on the bridge deck.
cscd= 1.0 # structural factor.
pw= cscd*cfx*qp

# Compute reference area
deckLength= 634.0
Aref= totalDepth*deckLength

# Compute transverse wind force
Fw= pw*Aref
fw= Fw/deckLength

# Compute vertical wind force.
# Compute vertical pressure distribution.
## Here d_tot must be computed disregarding the traffic
## and any bridge equipment
dtotVP= deckDepth
alph= math.radians(10)
beta= 0.0
## Vertical force coefficient.
cfz= ec1_wind.get_bridge_deck_vertical_force_coefficient(b= deckWidth, dtotVP= dtotVP, alpha= alph, beta= beta)
Fwz= cscd*cfz*qp*deckWidth*deckLength
vpd= ec1_wind.get_vertical_pressure_distribution(terrainCategory= terrainCategory, x0= -deckWidth/2.0, x1= deckWidth/2.0, dtotVP= dtotVP, z= z, vb= vb, zMax= zMax, rho= rho, k1= k1, c0= c0, alpha= alph, beta= beta)
fwz= Fwz/deckLength

# Check results.
fwRef= 8.538e3
ratio_fw= abs(fw-fwRef)/fwRef
fwzRef= 15.259e3
ratio_fwz= abs(fwz-fwzRef)/fwzRef

'''
print('Value of the probability factor c_prob= ', c_prob)
print('Basic velocity vb= ', vb, 'm/s')
print('vm= ', vm)
print('qb= ', qb)
print('cr= ', cr)
print('Turbulence intensity: Iv= ', Iv)
print('Peak velocity pressure: qp= ', qp/1e3, 'kPa')
print('Exposure factor: ce= ', ce)
print('total depth: dtot= ', totalDepth, 'm')
print('depth to width ratio: depthRatio= ', depthRatio)
print('Force coefficient: cfx0= ', cfx0)
print('Wind pressure on the bridge deck: pw= ', pw/1e3,'kN/m2')
print('Exposed area: Aref= ', Aref,'m2')
print('Transverse wind force Fw= ', Fw/1e3,'kN')
print('Transverse wind force per unit length fw= ', fw/1e3,'kN/m')
print('ratio_fw= ', ratio_fw)
print('Vertical force coefficien: cfz= ', cfz)
print('Vertical force: Fwz= ', Fwz/1e3,'kN')
print('Vertical force per unit length: fwz= ', fwz/1e3, 'kN/m')
print('ratio_fwz= ', ratio_fwz)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio_fw)<1e-2) and (abs(ratio_fwz)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
