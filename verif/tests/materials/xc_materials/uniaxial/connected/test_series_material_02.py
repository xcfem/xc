# -*- coding: utf-8 -*-
'''SeriesMaterial trivial test.''' 

from __future__ import print_function

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Materials.
# Soil type material no-tension material.
mat1= typical_materials.defElastNoTensMaterial(preprocessor, "mat1",E= 1e12)
sampleStrainStressCurve= [(0.02, 13527), (0.05, 15957), (0.1, 16100), (0.270833333, 16100.41376)]
mat2= typical_materials.defMultiLinearMaterial(preprocessor, "1005", sampleStrainStressCurve)
materialsToConnect= [mat1.name, mat2.name]
seriesMaterial= typical_materials.defSeriesMaterial(preprocessor, 'seriesMaterial', materialsToConnect)

# Spring to avoid zero stiffness when soil is in tension.
k= typical_materials.defElasticMaterial(preprocessor, "k",E= 1e2)

ratio= abs(seriesMaterial.numMaterials-len(materialsToConnect))
# Compute stresses.
stresses= list()
strains= [0.0, 0.005, 0.01, 0.015, 0.02, 0.0275, 0.035, 0.0425, 0.05, 0.0625, 0.07500000000000001, 0.08750000000000001, 0.1, 0.14270833325, 0.1854166665, 0.22812499975, 0.270833333, 0.22812499975, 0.1854166665, 0.14270833325, 0.1, 0.08750000000000001, 0.07500000000000001, 0.0625, 0.05, 0.0425, 0.035, 0.0275, 0.02, 0.015, 0.01, 0.005, 0.0, -0.005, -0.01, -0.015, -0.02, -0.0275, -0.035, -0.0425, -0.05, -0.0625, -0.07500000000000001, -0.08750000000000001, -0.1, -0.14270833325, -0.1854166665, -0.22812499975, -0.270833333, -0.22812499975, -0.1854166665, -0.14270833325, -0.1, -0.08750000000000001, -0.07500000000000001, -0.0625, -0.05, -0.0425, -0.035, -0.0275, -0.02, -0.015, -0.01, -0.005, 0.0, 0.005, 0.01, 0.015, 0.02, 0.0275, 0.035, 0.0425, 0.05, 0.0625, 0.07500000000000001, 0.08750000000000001, 0.1, 0.14270833325, 0.1854166665, 0.22812499975, 0.270833333]

## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Define mesh.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,0)

## Define element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= seriesMaterial.name
elements.dimElem= 2 # Dimension of element space
zl= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
elements.defaultMaterial= k.name
spring= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

# Define load.
## Load definition.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "linear_ts")
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([1,0,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

## Solution recipe
dispIncrement= .001
maxU= 0.0
numSteps= 5
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= numSteps, maxNumIter= 100)
solProc.setup()
integrator= solProc.integrator
material= zl.getMaterials()[0]

prevStrain= 0.0
for strain in strains:
    strainIncrement= strain-prevStrain
    integrator.increment= strainIncrement/numSteps
    analOk= solProc.solve()
    if(analOk!=0):
        print("Can't solve.")
        exit(1)
    matStrain= material.getStrain()
    deltax= n2.getDisp[0]
    stress= material.getStress()
    stresses.append(stress)
    prevStrain= deltax

refStresses= [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -3381.7477127549337, -6763.49542550987, -10145.243138264801, -13526.990851019738, -14134.498855105594, -14741.998805898094, -15349.498756690602, -15956.998707483106, -15992.749954260735, -16028.49995415849, -16064.249954056246, -16099.999953954, -16100.103439961005, -16100.206879961006, -16100.310319961005, -16100.413759961004, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# Compute error.
err= 0
for s, rs in zip(stresses, refStresses):
    err+= (s-rs)**2
err= math.sqrt(err)

'''
print(strains)
print(stresses)
print(err)
print(seriesMaterial.numMaterials)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio==0) and (abs(err)<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('strain')
# xi= range(0, len(strains))
# plt.plot(xi, strains, color= 'blue')
# plt.show()

# plt.title(fname)
# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(strains, stresses, color= 'blue')
# plt.show()
