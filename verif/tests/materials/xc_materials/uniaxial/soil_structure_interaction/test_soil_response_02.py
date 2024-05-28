# -*- coding: utf-8 -*-
''' Pressure-displacement diagram verification test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from geotechnics import earth_pressure
from materials import typical_materials

phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000)

depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2

## Force-displacement points.
samplePoints, initStrain= earth_pressure.get_horizontal_soil_reaction_diagram(depth= depth, tributaryArea= tributaryArea, gamma= rankineSoil.gamma(), Ka= rankineSoil.Ka(), K0= rankineSoil.K0Jaky(), Kp= rankineSoil.Kp(), Kh= 30e6)

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Material definition
soilResponse= typical_materials.defHorizontalSoilReactionMaterial(preprocessor, name= "soilResponse", samplePoints= samplePoints, initStrain= -initStrain)

# Invert sample.
samplePoints= [ (-x,-y) for (x,y) in samplePoints]
initStrain= -initStrain

# Compute stresses.
err= 0.0
stresses= list()
for ss in samplePoints:
    sampleDisp= ss[0]-initStrain
    soilResponse.setTrialStrain(sampleDisp, 0.0)
    soilResponse.commitState()
    stress= soilResponse.getStress()
    sampleStress= ss[1]
    err+= (sampleStress-stress)**2
    stresses.append(stress)

'''
print('initStrain= ', initStrain)
print('sample points: ', samplePoints)
print(err)
'''

import os

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # FIGURES & REPORTS
# unzipped= [list(t) for t in zip(*samplePoints)]
# sampleStrains= unzipped[0]
# sampleStresses= unzipped[1]
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('displacement')
# plt.ylabel('force')
# plt.plot(sampleStrains, stresses, color= 'blue')
# plt.plot(sampleStrains, sampleStresses, 'ro')
# plt.show()


