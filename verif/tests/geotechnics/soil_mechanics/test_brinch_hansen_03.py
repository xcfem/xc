# -*- coding: utf-8 -*-
'''
Verification of the Brinch Hansen formula for frictional soils.

See Brinch Hansen. A general formula for bearing capacity. The Danish Geotechnical Institute. Bulletin 11. Copenhagen 1961. Example on page 45
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from scipy.constants import g
from geotechnics import frictional_soil as fs
from geotechnics import frictional_cohesive_soil as fcs
import math


# Long-term bearing capacity.
phi= math.radians(30.0)
rho= (2.2-1.0)/g
cohesiveSoil= fcs.FrictionalCohesiveSoil(phi, c= 0.0, rho= rho)
frictionalSoil= fs.FrictionalSoil(phi, rho= rho)

D= 2.0 # Foundation depth
Beff= 5.5 # Effective foundation width
Leff= 9.0 # Effective foundation length
V= 3000.0 # Vertical load
H= 225.0 # Horizontal load.
q= 4.4

quCohesiveSoil= cohesiveSoil.qu(q= q, D= D, Beff= Beff, Leff= Leff, Vload= V, HloadB= H, HloadL= 0.0, NgammaCoef= 1.8)
quFrictionalSoil= frictionalSoil.qu(q= q, D= D, Beff= Beff, Leff= Leff, Vload= V, HloadB= H, HloadL= 0.0, NgammaCoef= 1.8)
quRef= 145.52314422500203

err= math.sqrt((quCohesiveSoil-quFrictionalSoil)**2+(quCohesiveSoil-quRef)**2)
testOK= abs(err)<1e-6

'''
print(quCohesiveSoil)
print(quFrictionalSoil)
print(quRef)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
