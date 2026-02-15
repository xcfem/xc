# -*- coding: utf-8 -*-
''' Trivial test of CreepMaterial material constitutive model.  

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials.ec2 import EC2_materials
from materials import typical_materials

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Units: kN, mm
kN = 1
mm = 1
GPa = kN/mm**2
MPa = 0.001*GPa
 
ft = 3*MPa # concrete tensile strength (tension is positive)
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
tDry = 14 # days
tcast = 0 # analysis time corresponding to concrete casting (in days; minimum value 2.0)

# Creep and shrinkage parameters.
Tcr = 28 # creep model age (in days)
epsshu = -600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
phiu = 3.0 # ultimate creep coefficient as per ACI 209R-92
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # fitting parameter of the creep time evolution function as per ACI 209R-92. Based on section dimensions
csParameters= typical_materials.def_creep_and_shrinkage_parameters(tcr= Tcr, epsshu= epsshu, epssha= psish, epscru= phiu, epscra= psicr1, epscrd= psicr2)

# Encapsulated concrete.
concrAux= EC2_materials.C25
epsc0= concrAux.epsilon0()
fpc= concrAux.fmaxK()
Ec0= 2.0*fpc/epsc0
concrete= typical_materials.defConcrete02IS(preprocessor=preprocessor,name='concrete', Ec0= Ec0, epsc0= epsc0, fpc= fpc, fpcu= 0.85*concrAux.fmaxK(), epscu= concrAux.epsilonU(), ratioSlope= 0.1, ft= ft, Ets= concrAux.E0()/19.0)

## Concrete able to creep.
creepMaterial= typical_materials.defCreepMaterial(preprocessor= preprocessor, name= 'creepMaterial', encapsulatedConcrete= concrete, beta= beta, age= tDry, tcast= tcast, csParameters= csParameters)
error= (creepMaterial.beta-beta)**2 # 1
error+= (creepMaterial.age-tDry)**2 # 2
error+= (creepMaterial.tcast-tcast)**2 # 3
error= math.sqrt(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

