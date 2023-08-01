# -*- coding: utf-8 -*-
''' Trivial test for LayeredShellFiberSection material.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

import math
import xc
from model import predefined_spaces
from materials import typical_materials



# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define materials
concretePlaneStress= typical_materials.defPlasticDamageConcretePlaneStress(preprocessor, name= 'concretePlaneStress', E= 25743.0e6, nu= 0.25, ft= 2.07e6, fc= 20.7e6, beta= 0.6, Ap= 0.1, An= 2.0, Bn= 0.75) # <beta Ap An Bn> 0.6 0.5 2.0 0.75

plateFromPlaneStress= typical_materials.defPlateFromPlaneStress(preprocessor, name= 'plateFromPlaneStress', underlyingMaterial= concretePlaneStress, outOfPlaneShearModulus= 1.25e10)

steel7= typical_materials.defSteel02(preprocessor,name= 'steel7',E= 202.7e9,fy= 379e6,b= .01, params= [18.5, 0.925, 0.15], initialStress= 0.0)
steel8= typical_materials.defSteel02(preprocessor,name= 'steel8',E= 200.6e9, fy= 392e6, b= .01, params= [18.5, 0.925, 0.15], initialStress= 0.0)

plateRebar10= typical_materials.defPlateRebar(preprocessor, name= 'plateRebar10', uniaxialMaterial= steel8, angle= math.radians(90))
plateRebar11= typical_materials.defPlateRebar(preprocessor, name= 'plateRebar11', uniaxialMaterial= steel8, angle= 0.0)

conc= 'plateFromPlaneStress'
steel11= plateRebar11.name
layeredShell= typical_materials.defLayeredShellFiberSection(preprocessor, name= 'layeredShell', materialThicknessPairs= [(conc, 0.0125), (steel11, 0.0002403), (steel11, 0.0003676), (conc, 0.024696), (conc, 0.024696), (conc, 0.024696), (conc, 0.024696), (steel11, 0.0003676), (steel11, 0.0002403), (conc, 0.0125)])

tangentInitialStiffness= layeredShell.getInitialTangentStiffness()
tangentStiffness= layeredShell.getTangentStiffness()

tangentInitialStiffnessRef= xc.Matrix([[3.6429e+09, 8.49752e+08, 0, -2.51765e-08, -6.29411e-09, 0, 0, 0],
[8.49752e+08, 3.39901e+09, 0, -6.29411e-09, -2.51765e-08, 0, 0, 0], 
[0, 0, 1.27463e+09, 0, 0, -1.03725e-08, 0, 0], 
[2.51765e-08, 6.29411e-09, 0, -4.84233e+06, -1.06e+06, 0, 0, 0], 
[6.29411e-09, 2.51765e-08, 0, -1.06e+06, -4.24e+06, 0, 0, 0], 
[0, 0, 1.03725e-08, 0, 0, -1.59e+06, 0, 0], 
[0, 0, 0, 0, 0, 0, 1.5473e+09, 0], 
[0, 0, 0, 0, 0, 0, 0, 1.5473e+09]])

tangentStiffnessRef= tangentInitialStiffnessRef

tangentInitialRefNorm= tangentInitialStiffnessRef.Norm()
tangentRefNorm= tangentInitialRefNorm
ratio1= (tangentInitialStiffness-tangentInitialStiffnessRef).Norm()/tangentInitialRefNorm
ratio2= (tangentStiffness-tangentStiffnessRef).Norm()/tangentRefNorm

'''
print('R0= ', steel7.R0)
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
