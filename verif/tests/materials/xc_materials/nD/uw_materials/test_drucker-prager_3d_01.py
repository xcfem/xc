# -*- coding: utf-8 -*-
# Home made test
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

mTo= mat.mTo
refValue=  math.sqrt(2.0/3.0)*mat.sigY/mat.mrho
ratio1= abs(mTo-refValue)/refValue

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
