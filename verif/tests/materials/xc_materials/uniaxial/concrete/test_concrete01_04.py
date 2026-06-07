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
from misc_utils import log_messages as lmsg

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

# Run 2. Unidirectional incremental strain in compression only.
# See https://opensees.berkeley.edu/OpenSees/manuals/comparisonManual/2462.htm
dispIncrement= -.00025 # Displacement increment.
minU = -0.035  # Max displacement.
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= 1, maxNumIter= 100, printFlag= 0)
solProc.setup()

concTrussMaterial= conc01Truss.getMaterial()
ok= 0
strains= list()
concreteStrains= list()
concreteNormalizedStresses= list()
currentDisp= 0.0
analysis= solProc.getAnalysis()
numSteps= 0
while ok == 0 and currentDisp >= minU:

    ok= analysis.analyze(1)

    # if the analysis fails try initial tangent iteration
    if ok != 0:
        lmsg.error("modified newton failed at disp: "+str(currentDisp))
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

concreteStrainsRef= [-0.00025, -0.0005, -0.00075, -0.001, -0.00125, -0.0015, -0.00175, -0.002, -0.0022500000000000007, -0.0024999999999999996, -0.0027499999999999886, -0.0029999999999999897, -0.003249999999999985, -0.0034999999999999806, -0.0037499999999999808, -0.003999999999999982, -0.004249999999999977, -0.00449999999999998, -0.004749999999999983, -0.0049999999999999845, -0.00524999999999999, -0.005499999999999986, -0.005749999999999989, -0.00599999999999999, -0.006249999999999991, -0.006499999999999987, -0.0067499999999999895, -0.006999999999999993, -0.007249999999999994, -0.00749999999999999, -0.007749999999999993, -0.007999999999999997, -0.008249999999999997, -0.008499999999999999, -0.008749999999999996, -0.008999999999999996, -0.009249999999999996, -0.009499999999999991, -0.009749999999999995, -0.009999999999999998, -0.010249999999999999, -0.010499999999999994, -0.010749999999999997, -0.011000000000000001, -0.011250000000000001, -0.011500000000000003, -0.011749999999999998, -0.012000000000000002, -0.012250000000000006, -0.012500000000000008, -0.01275000000000001, -0.013000000000000006, -0.01325000000000001, -0.013500000000000014, -0.013750000000000016, -0.014000000000000016, -0.014250000000000016, -0.014500000000000016, -0.014750000000000017, -0.015000000000000017, -0.015250000000000017, -0.015500000000000017, -0.015750000000000017, -0.016000000000000018, -0.016250000000000018, -0.016500000000000018, -0.01675000000000002, -0.01700000000000002, -0.01725000000000002, -0.01750000000000002, -0.01775000000000002, -0.01800000000000002, -0.01825000000000002, -0.01850000000000002, -0.01875000000000002, -0.01900000000000002, -0.01925000000000002, -0.01950000000000002, -0.01975000000000002, -0.02000000000000002, -0.02025000000000002, -0.02050000000000002, -0.020750000000000022, -0.021000000000000022, -0.021250000000000022, -0.021500000000000023, -0.021750000000000023, -0.022000000000000023, -0.022250000000000023, -0.022500000000000023, -0.022750000000000024, -0.023000000000000024, -0.023250000000000024, -0.023500000000000024, -0.023750000000000025, -0.024000000000000025, -0.024250000000000025, -0.024500000000000025, -0.024750000000000025, -0.025000000000000026, -0.025250000000000026, -0.025500000000000026, -0.025750000000000026, -0.026000000000000027, -0.026250000000000027, -0.026500000000000027, -0.026750000000000027, -0.027000000000000027, -0.027250000000000028, -0.027500000000000028, -0.027750000000000028, -0.02800000000000003, -0.02825000000000003, -0.02850000000000003, -0.02875000000000003, -0.02900000000000003, -0.02925000000000003, -0.02950000000000003, -0.02975000000000003, -0.03000000000000003, -0.03025000000000003, -0.03050000000000003, -0.03075000000000003, -0.03100000000000003, -0.03125000000000003, -0.03150000000000003, -0.03175000000000003, -0.03200000000000003, -0.03225000000000003, -0.03250000000000003, -0.03275000000000003, -0.03300000000000003, -0.03325000000000003, -0.03350000000000003, -0.03375000000000003, -0.03400000000000003, -0.03425000000000003, -0.03450000000000003, -0.03475000000000003, -0.03500000000000003]

concreteNormalizedStressesRef= [-0.17212732662959762, -0.32800965325919523, -0.46764697988879284, -0.5910393065183905, -0.698186633147988, -0.7890889597775856, -0.8637462864071834, -0.9221586130367811, -0.9643259396663787, -0.9902482662959763, -0.9999255929255739, -0.9959250520111208, -0.991418806345381, -0.9869125606796413, -0.9824063150139013, -0.9779000693481613, -0.9733938236824213, -0.9688875780166815, -0.9643813323509415, -0.9598750866852015, -0.9553688410194614, -0.9508625953537215, -0.9463563496879817, -0.9418501040222417, -0.9373438583565017, -0.9328376126907617, -0.9283313670250217, -0.9238251213592819, -0.9193188756935419, -0.9148126300278019, -0.9103063843620619, -0.9058001386963219, -0.901293893030582, -0.8967876473648421, -0.8922814016991021, -0.8877751560333623, -0.8832689103676223, -0.8787626647018824, -0.8742564190361424, -0.8697501733704025, -0.8652439277046625, -0.8607376820389225, -0.8562314363731826, -0.8517251907074426, -0.8472189450417027, -0.8427126993759627, -0.838206453710223, -0.8337002080444827, -0.8291939623787428, -0.8246877167130029, -0.8201814710472629, -0.8156752253815229, -0.811168979715783, -0.806662734050043, -0.8021564883843031, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8]

# Check results.
errStrains= 0.0
for strain, refStrain in zip(concreteStrains, concreteStrainsRef):
    errStrains+= (strain-refStrain)**2
errStrains= math.sqrt(errStrains)
    
errNormalizedStress= 0.0
for ns, nsRef in zip(concreteNormalizedStresses, concreteNormalizedStressesRef):
    errNormalizedStress+= (ns-nsRef)**2
errNormalizedStress= math.sqrt(errNormalizedStress)

'''
print(errStrains)
print(errNormalizedStress)
'''

testOK= (errStrains<1e-10) and (errNormalizedStress<1e-10)
    
import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# import matplotlib.pyplot as plt

# plt.title("Concrete01 behavior. Monotonic compression.")
# plt.xlabel('Strain')
# plt.ylabel('Stress/|fc|')
# plt.grid()
# plt.plot(strains, concreteNormalizedStresses, '-')
# plt.show()
