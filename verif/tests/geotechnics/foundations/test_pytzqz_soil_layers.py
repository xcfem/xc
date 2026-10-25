# -*- coding: utf-8 -*-
'''Trivial test for PyTzQzPile class.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from scipy.constants import g
from model import predefined_spaces
from materials.ec2 import EC2_materials
from geotechnics.foundations import pytzqz_pile
from geotechnics import frictional_soil as fs

# Data
## Soil data.
zGround= 0.0
## Dummy soil data.
soilRows= list()
for i in range(0, 10):
    dummyPhi= math.radians(30.0+i)
    dummySoil= fs.FrictionalSoil(phi= dummyPhi, rho= (i+1)*1e4/g, gammaMPhi= 1.0)
    # select pult definition method for p-y curves
    # API --> 1
    # Brinch Hansen --> 2
    dummySoil.puSwitch= 1
    # variation in coefficent of subgrade reaction with depth for p-y curves
    # API linear variation (default)   --> 1
    # modified API parabolic variation --> 2
    dummySoil.kSwitch= 1
    depth= -(i+1)
    soilRows.append((depth, dummySoil))

soilLayers= pytzqz_pile.SoilLayers(soilProfile= soilRows, groundLevel= zGround, waterTableZ= None)

depths= [-(i+0.5) for i in range(0, 9)]

vStresses= list()
for depth in depths:
    verticalStress= soilLayers.getVerticalStress(depth)
    vStresses.append(verticalStress/1e3)

zWaterTable= -5.0
wetSoilLayers= pytzqz_pile.SoilLayers(soilProfile= soilRows, groundLevel= zGround, waterTableZ= zWaterTable, waterDensity= 1e4/g)

waterPressures= list()
wetVStresses= list()
for depth in depths:
    verticalStress= wetSoilLayers.getVerticalStress(depth)
    wetVStresses.append(verticalStress/1e3)

# Check results.
refVStresses= [-5, -20, -45, -80, -125, -180, -245, -320, -405]
error= 0.0
for vS, rVs in zip(vStresses, refVStresses):
    error+= (vS-rVs)**2
refWetVStresses= [-5, -20, -45, -80, -125, -175, -230, -295, -370]
for vS, rVs in zip(wetVStresses, refWetVStresses):
    error+= (vS-rVs)**2

error= math.sqrt(error)

'''
print(refWetVStresses)
print(wetVStresses)
print(error)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if error<1e-8:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# import matplotlib.pyplot as plt
# ## Units.
# plt.title("Vertical stresses.")
# plt.xlabel('$\sigma_v$ (kPa)')
# plt.ylabel('Depth (m)')
# plt.plot(wetVStresses, depths, 'o')
# plt.grid()
# plt.show()
