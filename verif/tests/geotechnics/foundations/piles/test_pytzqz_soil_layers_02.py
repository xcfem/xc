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
from geotechnics.foundations import pytzqz_soil_layers
from geotechnics.foundations import pytzqz_pile
from geotechnics import frictional_soil as fs
from geotechnics import frictional_cohesive_soil as fcs

# Data
## Soil data.
soilRows= list()
soilA= fcs.FrictionalCohesiveSoil(phi= math.radians(17), c= 20e3, rho= 19.89e3/g)
soilA.puSwitch= None # Not used for clays (phi<28.8 degrees).
soilA.kSwitch= None # Not used for clays (phi<28.8 degrees).
soilRows.append((-18.0, soilA))
soilB= fcs.FrictionalCohesiveSoil(phi= math.radians(24), c= 35e3, rho= 19.89e3/g)
soilB.puSwitch= None # Not used for clays (phi<28.8 degrees).
soilB.kSwitch= None # Not used for clays (phi<28.8 degrees).
soilRows.append((-72.0, soilB))
zGround= 0.0
soilLayers= pytzqz_soil_layers.SoilLayers(soilProfile= soilRows, groundLevel= zGround, waterTableZ= None)

testZs= [0.0]+[row[0] for row in soilRows]
testDepths= [0.5, 2, 4, 6]
z0= testZs[0]
for z1 in testZs[1:]:
    avgZ= (z0+z1)/2.0
    testDepths.append(soilLayers.getDepth(avgZ))
    z0= z1

# Compute results.
pult_results= dict()
y50_results= dict()
for depth in testDepths:
    pult, y50= soilLayers.getPyParametersAtDepth(depth, pileDiameter= 1.0, tributaryLength= 1.0)
    pult_results[depth]= pult
    y50_results[depth]= y50

# Check results.
ref_pult_results= {0.5: 74945.0, 2: 119780.0, 4: 179560.0, 6: 180000.0, 9.0: 180000.0, 45.0: 315e3}
ref_y50_results= {0.5: 0.05, 2: 0.05, 4: 0.05, 6: 0.05, 9.0: 0.05, 45.0: 0.05}
err_pult= 0.0
err_y50= 0.0
for depth in ref_pult_results:
    err_pult+= (ref_pult_results[depth]-pult_results[depth])**2
    err_y50+= (ref_y50_results[depth]-y50_results[depth])**2
err_pult= math.sqrt(err_pult)
err_y50= math.sqrt(err_y50)
testOK= (err_pult<1e-8) and (err_y50<1e-8)

'''
print('test depths: ', testDepths)
print('computed pult values: ', pult_results)
print('error in pult computation: ', err_pult)
print('reference pult values: ', ref_pult_results)
print('error in y50 computation: ', err_y50)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    

