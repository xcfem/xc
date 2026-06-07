# -*- coding: utf-8 -*-
''' Test based on the material comparison described in the following page:

https://opensees.berkeley.edu/OpenSees/manuals/comparisonManual/2622.htm
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Material properties
# See here: https://opensees.berkeley.edu/OpenSees/manuals/comparisonManual/2622.htm
fc= -4.0 # ksi Nominal concrete compressive strength.
absFc= math.fabs(fc)
Ec= 57*math.sqrt(absFc*1e3) # ksi =3605ksi Concrete Elastic Modulus.
fc1C= 1.25*fc # (= -5.0ksi)
absFc1C= math.fabs(fc1C)
eps1C= 2.*fc1C/Ec # (-0.0028) strain at maximum stress.
fc2C= fc # = -4.0ksi ultimate stress.
eps2C= 5*eps1C # = - 0.0139 strain at ultimate stress.

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)
l= 1 # Distance between nodes
n1= nodes.newNodeXY(0, 0)
n2= nodes.newNodeXY(l, 0)

# Materials definition
## Define concrete material.
conc01= typical_materials.defConcrete01(preprocessor, "conc01",epsc0= eps1C, fpc= fc1C, fpcu= fc2C, epscu= eps2C)
## Define elastic material (see Numerical Model for Material Behavior here:
## https://opensees.berkeley.edu/OpenSees/manuals/comparisonManual/2612.htm
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E= 1)

# Elements definition
## Dummy elastic truss.
modelSpace.setDefaultMaterial(elast)
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
elasticTruss= modelSpace.newElement("Truss",nodeTags= [n1.tag,n2.tag])
elasticTruss.sectionArea= 1
## Concrete01 truss.
modelSpace.setDefaultMaterial(conc01)
conc01Truss= modelSpace.newElement("Truss",nodeTags= [n1.tag,n2.tag])
conc01Truss.sectionArea= 1

# Constraints.
modelSpace.fixNode('00', n1.tag) # Both displacements constrained.
modelSpace.fixNode('F0', n2.tag) # Y displacement constrained.

# Set parameters for displacement control analysis.

## Define load.
### Create a linear time series
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
### Create a load pattern.
lp= modelSpace.newLoadPattern(name= 'lp', setCurrent= True)
lp.newNodalLoad(n2.tag, xc.Vector([1,0]))
modelSpace.addLoadCaseToDomain(lp.name)

# Run 1 Unidirectional incremental strain in tension.
# See https://opensees.berkeley.edu/OpenSees/manuals/comparisonManual/2462.htm
dispIncrement= .01 # Displacement increment.
maxU = 0.02  # Max displacement.
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= 1, maxNumIter= 100, printFlag= 0)
solProc.setup()

concTrussMaterial= conc01Truss.getMaterial()
ok= 0
strains= list()
numSteps= 0
concreteStrains= list()
concreteNormalizedStresses= list()
currentDisp= 0.0
analysis= solProc.getAnalysis()
while ok == 0 and currentDisp < maxU:

    ok= analysis.analyze(1)

    # if the analysis fails try initial tangent iteration
    if ok != 0:
        print("modified newton failed at disp: ", currentDisp)
        break
    else:
        # Compute strain.
        strain= currentDisp/l
        strains.append(strain)
        # Compute material stress.
        concStrain= concTrussMaterial.getStrain()
        concStress= concTrussMaterial.getStress()
        concreteStrains.append(concStrain)
        concreteNormalizedStresses.append(concStress/absFc1C)
        # Increment step number.
        numSteps+= 1

    currentDisp= n2.getDisp[0]

refStrains= [0, dispIncrement, 2*dispIncrement]
refConcreteNormalizedStresses= numSteps*[0.0] # Concrete01 has zero tensile strength.

# Check results.
## Strains.
strainsErr= 0.0
for strain, refStrain in zip(strains, refStrains):
    strainsErr+= (strain-refStrain)**2
strainsErr= math.sqrt(strainsErr)
## Stresses.
concreteNormalizedStressesErr= 0.0
for stress, refStress in zip(concreteNormalizedStresses, refConcreteNormalizedStresses):
    concreteNormalizedStressesErr+= (stress-refStress)**2
concreteNormalizedStressesErr= math.sqrt(concreteNormalizedStressesErr)

'''
print('strains: ', strains, strainsErr)
print('normalized concrete stresses: ', concreteNormalizedStresses, concreteNormalizedStressesErr)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(strainsErr)<1e-12) and (abs(concreteNormalizedStressesErr)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# import matplotlib.pyplot as plt
# plt.title("Concrete01 behavior. Monotonic tension.")
# plt.xlabel('Strain')
# plt.ylabel('Stress/|fc|')
# plt.grid()
# plt.plot(strains, concreteNormalizedStresses, '-')
# plt.show()
