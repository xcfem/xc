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
 
Ec = 25*GPa
fc = 28*MPa
ft = 3*MPa
beta = 0.4 # Recommended value
tDry = 14 # days
epsshu = 600e-6 # Ultimate shrinkage strain
psish = 75.4218 # Based on section dimensions
Tcr = 28 # days
phiu = 3.0 # Ultimate creep factor
psicr1 = 1.0 # Recommended value
psicr2 = 75.4218 # Based on section dimensions
tcast = 0
 
tdConcrete= typical_materials.defTDConcrete(preprocessor= preprocessor, name= 'tdConcrete',fpc= -fc,ft= ft, Ec= Ec, beta= beta, age= tDry, epsshu= -epsshu, epssha= psish, tcr= Tcr, epscru= phiu, epscra= psicr1, epscrd= psicr2, tcast= tcast)

# Check Et value.
ratio0= abs(tdConcrete.Et-Ec)/Ec

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

