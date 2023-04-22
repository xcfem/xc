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

# Cam-Clay yield surface.
camClayYieldSurface= xc.CamClayYieldSurface()

# Cam-Clay potential surface.
camClayPotentialSurface= xc.CamClayPotentialSurface(1.2)

# Scalar evolution law void ratio, Lambda, Kappa 
es1= xc.EvolutionLaw_NL_Ep(0.85, 0.19, 0.06)
ratio1= abs(es1.eo-0.85)/0.85
ratio2= abs(es1.lmbda-0.19)/0.19
ratio3= abs(es1.kappa-0.06)/0.06

# Initial stress.
sts= xc.stresstensor([0.10, 0, 0,0, 0.10, 0, 0, 0, 0.10])

#________________po
epState= xc.EPState()
epState.stress= sts # Set trial stress.
epState.scalarVars= [200.1] # scalar variables.
epState.tensorVars= [] # tensorial variables.

# Cam-clay material
camClayMat= typical_materials.defTemplate3Dep(preprocessor, name= "camClay", elasticMaterial= elasticMat, yieldSurface= camClayYieldSurface, potentialSurface= camClayPotentialSurface, elasticPlasticState= epState, scalarEvolutionLaws= [es1], tensorialEvolutionLaws= [])

'''
print('sts= ', sts)
print("ratio1= ", ratio1)
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
