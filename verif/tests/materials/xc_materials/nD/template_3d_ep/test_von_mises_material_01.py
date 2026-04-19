# -*- coding: utf-8 -*-
''' Trivial test checking the creation of von Mises material by means
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

# von Mises yield surface.
vonMisesYieldSurface= xc.VonMisesYieldSurface()

# Von Mises potential surface.
vonMisesPotentialSurface= xc.VonMisesPotentialSurface()

# Scalar evolution law: linear hardening coef = 1.1
es1= xc.EvolutionLaw_L_Eeq(1.10)
a= es1.a
ratio1= abs(a-1.1)/1.1

# Initial stress.
sts= xc.stresstensor([0, 0, 0,0, 0, 0, 0, 0, 0]) # no initial stress.

#________________po
epState= xc.EPState()
epState.stress= sts # Set trial stress.
epState.scalarVars= [20] # scalar variables.
epState.tensorVars= [] # tensorial variables.

# Make some tests.
f= vonMisesYieldSurface.f(epState)
ratio2= abs(f+400)/400
dQods= vonMisesPotentialSurface.dQods(epState)
dQodsNorm= dQods.frobeniusNorm()
ratio3= dQodsNorm


# von Mises material
vonMisesMat= typical_materials.defTemplate3Dep(preprocessor, name= "vonMises", elasticMaterial= elasticMat, yieldSurface= vonMisesYieldSurface, potentialSurface= vonMisesPotentialSurface, elasticPlasticState= epState, scalarEvolutionLaws= [es1], tensorialEvolutionLaws= [])

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
