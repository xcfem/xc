# -*- coding: utf-8 -*-
''' Trivial test checking the creation of Drucker-Prager potential and
    yield surfaces. '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.33 # Poisson's ratio
rho= 1.5 # Density

import xc

# Elastoplastic state.
epState= xc.EPState()

# Drucker-Prager potential surface.
druckerPragerPotentialSurface= xc.DruckerPragerPotentialSurface(0.1)
alpha2= druckerPragerPotentialSurface.alpha2
ratio1= abs(alpha2-0.1)/0.1

# Drucker-Prager yield surface.
druckerPragerYieldSurface= xc.DruckerPragerYieldSurface()
f= druckerPragerYieldSurface.f(epState)
ratio2= abs(f)

'''
print("alpha2= ", alpha2)
print("f= ", f)
'''
# Check results.
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio2)<1e-15):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
