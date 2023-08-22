# -*- coding: utf-8 -*-
''' Trivial test of TDConcrete material constitutive model.  

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

# Units: kN, mm
kN = 1
mm = 1
GPa = kN/mm**2
MPa = 0.001*GPa
 
Es = 200*GPa
elast= typical_materials.defElasticMaterial(preprocessor, "elast",Es)
 
Ec = 25*GPa # concrete modulus of elasticity
fc = -28*MPa # concrete compressive strength (compression is negative)
ft = 3*MPa # concrete tensile strength (tension is positive)
beta = 0.4 # Recommended value for the tension softening parameter (tension softening exponent).
tDry = 14 # days
epsshu = -600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
Tcr = 28 # creep model age (in days)
phiu = 3.0 # ultimate creep coefficient as per ACI 209R-92
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # fitting parameter of the creep time evolution function as per ACI 209R-92. Based on section dimensions
tcast = 0 # analysis time corresponding to concrete casting (in days; minimum value 2.0)

## Concrete able to creep.
tdConcrete= typical_materials.defTDConcrete(preprocessor= preprocessor, name= 'tdConcrete',fpc= fc,ft= ft, Ec= Ec, beta= beta, age= tDry, epsshu= epsshu, epssha= psish, tcr= Tcr, epscru= phiu, epscra= psicr1, epscrd= psicr2, tcast= tcast)
error= (tdConcrete.fpc-fc)**2 # 1
error+= (tdConcrete.ft-ft)**2 # 2
error+= (tdConcrete.Ec-Ec)**2 # 3
error+= (tdConcrete.beta-beta)**2 # 4
error+= (tdConcrete.age-tDry)**2 # 5
error+= (tdConcrete.epsshu-epsshu)**2 # 6
error+= (tdConcrete.epssha-psish)**2 # 7
error+= (tdConcrete.tcr-Tcr)**2 # 8
error+= (tdConcrete.epscru-phiu)**2 # 9
error+= (tdConcrete.epscra-psicr1)**2 # 10
error+= (tdConcrete.epscrd-psicr2)**2 # 11
error+= (tdConcrete.tcast-tcast)**2 # 12
error= math.sqrt(error)

# Check Et value.
ratio0= abs(tdConcrete.Et-Ec)/Ec

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12 and abs(ratio0)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

