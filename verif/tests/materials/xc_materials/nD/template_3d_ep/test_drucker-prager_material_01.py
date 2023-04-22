# -*- coding: utf-8 -*-
''' Trivial test checking the creation of Drucker-Prager material by means
    of the Template3Dep material.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from materials import typical_materials

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

E= 30e6 # Young modulus (psi)
nu= 0.33 # Poisson's ratio
rho= 1.5 # Density

elasticMat= typical_materials.defElasticIsotropic3d(preprocessor, "elastIso3d",E,nu,rho)

# Initial stress.
sts= xc.stresstensor([0.10, 0, 0,0, 0.10, 0, 0, 0, 0.10])

# Elastoplastic state.
epState= xc.EPState()
epState.stress= sts # Set trial stress.
epState.scalarVars= [0.2, 0.0]
epState.tensorVars= []

# Drucker-Prager potential surface.
druckerPragerPotentialSurface= xc.DruckerPragerPotentialSurface(0.1)
alpha2= druckerPragerPotentialSurface.alpha2
ratio1= abs(alpha2-0.1)/0.1

# Drucker-Prager yield surface.
druckerPragerYieldSurface= xc.DruckerPragerYieldSurface()
f= druckerPragerYieldSurface.f(epState)
ratio2= abs(f-0.06)/0.06

# Scalar evolution law: linear hardening coef = 1.1
es1= xc.EvolutionLaw_L_Eeq(1.1)
a= es1.a
ratio3= abs(a-1.1)/1.1

# where
#alpha = 2 sin(phi) / (3^0.5) / (3-sin(phi) ), phi is the friction angle
# and k is the cohesion
# Define material
druckerPragerMat= typical_materials.defTemplate3Dep(preprocessor, name= "druckerPrager", elasticMaterial= elasticMat, yieldSurface= druckerPragerYieldSurface, potentialSurface= druckerPragerPotentialSurface, elasticPlasticState= epState, scalarEvolutionLaws= [es1], tensorialEvolutionLaws= [])

'''
print('sts= ', sts)
print("alpha2= ", alpha2)
print("f= ", f)
print('a= ', a)
'''

# Check results.
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio2)<1e-15) and (abs(ratio3)<1e-15):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
