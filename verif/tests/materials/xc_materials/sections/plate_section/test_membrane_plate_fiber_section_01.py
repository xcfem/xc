# -*- coding: utf-8 -*-
''' Trivial test for MembranePlateFiberSection material.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from materials import typical_materials

L= 1.0 # Size of plate edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 400.0 # Temperature increment (Celsius degrees)
h= 0.2
A= L*h

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Materials definition
steel= typical_materials.defJ2PlateFibre(preprocessor=preprocessor, name='steel', E= E, nu= nu, fy=250e6,alpha=1e-6,rho= 0.0)
plate= typical_materials.defMembranePlateFiberSection(preprocessor,name='plate',h= h ,nDMaterial= steel)

tangentInitialStiffness= plate.getInitialTangentStiffness()
tangentStiffness= plate.getTangentStiffness()

tangentInitialStiffnessRef= xc.Matrix([[4.52769e+10, 1.35831e+10, 0, -1.85564e-07, 0, 0, 0, 0],
[1.35831e+10, 4.52769e+10, 0, -6.75029e-09, -7.21715e-08, 0, 0, 0],
[0, 0, 1.58469e+10, 0, 0, 1.94435e-08, 0, 0],
[7.21715e-08, 6.75029e-09, 0, -1.50923e+08, -4.52769e+07, 0, 0, 0],
[6.75029e-09, 7.21715e-08, 0, -4.52769e+07, -1.50923e+08, 0, 0, 0],
[0, 0, -1.94435e-08, 0, 0, -5.28231e+07, 0, 0],
[0, 0, 0, 0, 0, 0, 1.32058e+10, 0],
[0, 0, 0, 0, 0, 0, 0, 1.32058e+10]])

tangentStiffnessRef= tangentInitialStiffnessRef

tangentInitialRefNorm= tangentInitialStiffnessRef.Norm()
tangentRefNorm= tangentInitialRefNorm
ratio1= (tangentInitialStiffness-tangentInitialStiffnessRef).Norm()/tangentInitialRefNorm
ratio2= (tangentStiffness-tangentStiffnessRef).Norm()/tangentRefNorm

'''
print('tangentInitialStiffness= ', tangentInitialStiffness)
print('tangentInitialStiffnessRef= ', tangentInitialStiffnessRef)
print('ratio1= ', ratio1)
print('tangentStiffness= ', tangentStiffness)
print('tangentStiffnessRef= ', tangentStiffnessRef)
print('tangentRefNorm= ', tangentRefNorm)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) and (abs(ratio2)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
