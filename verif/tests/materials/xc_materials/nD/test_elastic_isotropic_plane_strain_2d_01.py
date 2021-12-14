# -*- coding: utf-8 -*-
# Home made test

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.33 # Poisson's ratio
rho= 1.5 # Density

import xc_base
import geom
import xc
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

elast2d= typical_materials.defElasticIsotropicPlaneStrain(preprocessor, "elast2d",E,nu,rho)
EE= elast2d.E
V= elast2d.nu
dens= elast2d.rho

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(E-EE)<1e-15) & (abs(V-nu)<1e-15) & (abs(dens-rho)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
