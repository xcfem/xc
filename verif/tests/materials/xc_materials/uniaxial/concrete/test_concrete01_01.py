# -*- coding: utf-8 -*-
''' Trivial Concrete01 material test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials

fc= -24 # Concrete compressive strength.
epsc0= -2e-3 # Maximum compressive strain for section in compression.
fcu= -4.0 # Strain under concrete compressive stress.
epsU= -5e-3 # Maximum compressive strain in concrete.

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Material definition
concrete= typical_materials.defConcrete01(preprocessor= preprocessor, name= "concrete", epsc0= epsc0, fpc= fc, fpcu= fcu, epscu= epsU)
E= concrete.getTangent()
E0= concrete.getInitialTangent()
Eref= 2*fc/epsc0

ratio0= abs(E-Eref)/Eref
ratio1= abs(E0-Eref)/Eref

''' 
print("fpc= ",fc)
print("epsc0= ",epsc0)
print("fpcu= ",fcu)
print("epsU= ",epsU)
print("E= ", E)
print("E0= ", E0)
print("Eref= ", Eref)
print('ratio0= ', ratio0)
print('ratio1= ', ratio1)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-17) & (ratio1<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


