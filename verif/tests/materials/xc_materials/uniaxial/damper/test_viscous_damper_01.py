# -*- coding: utf-8 -*-
'''Trivial test of ViscousDamper class.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import damper_materials

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Define viscous damper material
name= 'test'
k= 300
c= 280.3
alph= 0.3
lgap= 1e-3
nm= 2
relTol= 1e-5
absTol= 1e-9
maxIter= 25
vd= damper_materials.def_viscous_damper(modelSpace, name= name, k= k, c= c, alpha= alph, lgap= lgap, nm= nm, relTol= relTol, absTol= absTol, maxHalf= maxIter)

testOK= (vd.name==name)
testOK&= (vd.K==k)
testOK&= (vd.C==c)
testOK&= (vd.Alpha==alph)
testOK&= (vd.NM==nm)
testOK&= (vd.RelTol==relTol)
testOK&= (vd.AbsTol==absTol)
testOK&= (vd.MaxHalf==maxIter)


# print(testOK)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('strain')
# xi= range(0, len(lStrain))
# plt.plot(xi, lStrain, color= 'blue')
# plt.show()

# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(lStrain,lStress, color= 'blue')
# plt.show()
