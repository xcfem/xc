# -*- coding: utf-8 -*-
''' Testing the Drucker-Prager model

Based on the example in: https://opensees.github.io/OpenSeesDocumentation/user/manual/material/ndMaterials/DruckerPrager.html
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#---bulk modulus
k= 27777.78
#---shear modulus
G= 9259.26
#---yield stress
sigY= 5.0
#---failure surface and associativity
rho= 0.398
rhoBar= 0.398
#---isotropic hardening
Kinf= 0.0
Ko= 0.0
delta1= 0.0
#---kinematic hardening
H= 0.0
theta= 1.0
#---tension softening
delta2= 0.0
#---mass density
mDen= 1.7

import math
import xc
from model import predefined_spaces
from materials import typical_materials

# Define problem
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor

# Define material.
mat= typical_materials.defDruckerPrager3d(preprocessor, "druckerPrager3d",k= k, G= G, sigY= sigY, mRho= rho, mRhoBar= rhoBar, Kinf= Kinf, Ko= Ko, delta1= delta1, H= H, theta= theta, delta2= delta2, mDen= mDen)

# Check some quantities.
mTo= mat.mTo
refValue=  math.sqrt(2.0/3.0)*mat.sigY/mat.mrho
ratio1= abs(mTo-refValue)/refValue
materialTangent= mat.getTangent()
materialTangentNorm= materialTangent.Norm()
ratio2= abs(materialTangentNorm-88811.70198109705)/88811.70198109705
pAtm= mat.referencePressure
ratio3= abs(pAtm-101.0)/101.0

err= (mat.k-k)**2+(mat.G-G)**2+(mat.sigY-sigY)**2+(mat.mrho-rho)**2+(mat.mrhoBar-rhoBar)**2+(mat.Kinf-Kinf)**2+(mat.Ko-Ko)**2+(mat.delta1-delta1)**2+(mat.Hard-H)**2+(mat.theta-theta)**2+(mat.delta2-delta2)**2+(mat.rho-mDen)**2

err= math.sqrt(err)

'''
print('material tangent: ', materialTangent)
print('material norm: ', materialTangentNorm)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-12) and (ratio3<1e-12) and err<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
