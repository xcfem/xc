# -*- coding: utf-8 -*-
''' Trivial test of TDConcreteMC10 material constitutive model.  
TDConcreteMC10: uniaxial time-dependent concrete material object with linear 
behavior in compression, nonlinear behavior in tension 
(REF: Tamai et al., 1988) and creep and shrinkage according to fib Model 
Code 2010.

Based on the example: https://portwooddigital.com/2023/05/28/minimal-creep-and-shrinkage-example/
'''

from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

fc = -24.80 # mean 28-day cylinder compressive strength.
fct= 2.00 # the tensile strength (splitting or axial tensile strength).
Ec= 22820.0 # modulus of elasticity at the time of loading.
Ecm= 24950.0 # 28-day modulus.


# Creep Input from CreepParameters.txt
epsba= -0.000046 # ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
epsbb= 1.0 # fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
epsda= -0.001329 # product of εcds,0 and βRH, as per Model Code 2010.
epsdb= 726.77 # fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.

phiba= 0.1379 # parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
phibb= 1.0 # fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
phida= 2.544 # product of βdc(fcm) and β(RH), as per Model Code 2010.
phidb= 1782.0 # fitting constant within the drying creep time evolution function as per Model Code 2010.

cem = 1.0 # coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.

## Model Code 2010 shrinkage and creep parameters.
mc10CreepShrinkageParameters= typical_materials.def_mc10_creep_and_shrinkage_parameters(epsba= epsba, epsbb= epsbb, epsda= epsda, epsdb= epsdb, phiba= phiba, phibb= phibb, phida= phida, phidb= phidb, cem= cem)

error= (mc10CreepShrinkageParameters.epsba-epsba)**2 # 7
error+= (mc10CreepShrinkageParameters.epsbb-epsbb)**2 # 8
error+= (mc10CreepShrinkageParameters.epsda-epsda)**2 # 9
error+= (mc10CreepShrinkageParameters.epsdb-epsdb)**2 # 10
error+= (mc10CreepShrinkageParameters.phiba-phiba)**2 # 11
error+= (mc10CreepShrinkageParameters.phibb-phibb)**2 # 12
error+= (mc10CreepShrinkageParameters.phida-phida)**2 # 13
error+= (mc10CreepShrinkageParameters.phidb-phidb)**2 # 14
error+= (mc10CreepShrinkageParameters.cem-cem)**2 # 15

# Other Input Parameters:
tDry = 14.0
beta = 0.7 # can be changed (softening parameter)

## Concrete able to creep.
tdConcrete= typical_materials.defTDConcreteMC10(preprocessor= preprocessor, name= 'tdConcrete', fcm= fc, ft= fct, Ec= Ec, Ecm= Ecm, beta= beta, age= tDry, mc10CSParameters= mc10CreepShrinkageParameters, tcast= 0.0)

error+= (tdConcrete.fpc-fc)**2 # 1
error+= (tdConcrete.ft-fct)**2 # 2
error+= (tdConcrete.Ec-Ec)**2 # 3
error+= (tdConcrete.Ecm-Ecm)**2 # 4
error+= (tdConcrete.beta-beta)**2 # 5
error+= (tdConcrete.age-tDry)**2 # 6
error+= (tdConcrete.tcast)**2 # 15
error= math.sqrt(error)

# Check Et value.
ratio0= abs(tdConcrete.Et-Ec)/Ec

# print(ratio0)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12 and abs(ratio0)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

