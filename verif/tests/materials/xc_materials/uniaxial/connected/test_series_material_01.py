# -*- coding: utf-8 -*-
'''SeriesMaterial trivial test.''' 

from __future__ import print_function

import math
import numpy as np
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
mat1= typical_materials.defElastNoTensMaterial(preprocessor, "mat1",E= 1e12)
mat2= typical_materials.defElasticMaterial(preprocessor, "mat2", E= 1e3)
materialsToConnect= [mat1.name, mat2.name]
seriesMaterial= typical_materials.defSeriesMaterial(preprocessor, 'seriesMaterial', materialsToConnect)

ratio= abs(seriesMaterial.numMaterials-len(materialsToConnect))

strainStart= 0.0
strainStop= -.01
aValues= np.linspace(start= strainStart, stop= strainStop, num= 8)
strainValues= aValues.tolist()
bValues= strainValues[::-1] # up again.
strainValues.extend(bValues[1:])
cValues= np.linspace(start= strainStart, stop= -strainStop, num= 8) # go forward.
strainValues.extend(cValues[1:]) 
dValues= np.linspace(start= -strainStop, stop= strainStart, num= 8) # return to zero.
strainValues.extend(dValues[1:])

refStresses= [0.0, -1.4285714271428571, -2.8571428542857142, -4.285714281428572, -5.7142857085714285, -7.142857135714285, -8.571428562857143, -9.99999999, -8.571428562857143, -7.142857135714285, -5.7142857085714285, -4.285714281428572, -2.8571428542857142, -1.4285714271428571, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.9721522630525295e-31, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# Compute stresses.
err= 0.0
stresses= list()
for strain, refStress in zip(strainValues, refStresses):
    seriesMaterial.setTrialStrain(strain, 0.0)
    seriesMaterial.commitState()
    stress= seriesMaterial.getStress()
    err+= (refStress-stress)**2
    stresses.append(stress)

err= math.sqrt(err)

'''
print(seriesMaterial.numMaterials)
print(strainValues)
print(stresses)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0 and err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('strain')
# xi= range(0, len(strainValues))
# plt.plot(xi, strainValues, color= 'blue')
# plt.show()

# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(strainValues,stresses, color= 'blue')
# plt.show()
