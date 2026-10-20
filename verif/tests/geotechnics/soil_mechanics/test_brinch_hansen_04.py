# -*- coding: utf-8 -*-
'''
Verification of the Brinch Hansen formula for frictional soils. Results are
compared with those obtained with using Gemini LLM.

See Brinch Hansen. A general formula for bearing capacity. The Danish Geotechnical Institute. Bulletin 11. Copenhagen 1961. Example on page 45
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy.constants import g
from geotechnics import frictional_soil as fs

foundationSoilRho= 20.0e3/g
foundationSoilPhi= math.radians(30) # Internal friction angle.
foundationSoil= fs.FrictionalSoil(phi= foundationSoilPhi, rho= foundationSoilRho, gammaMPhi= 1.0)

D= 0.85 # Foundation depth
Beff= 0.2228740762672572 # Effective foundation width
Leff= 1.0 # Effective foundation length
V= 34.1288983e3 # Vertical load
H= 5.7190406932605695e3 # Horizontal load.
q= 0.0 # overburden load.

# Bearing capacity factors.
Nq= foundationSoil.Nq()
Ngamma= foundationSoil.Ngamma()
# Shape factors.
sq= foundationSoil.sq(Beff= Beff, Leff= Leff)
sgamma= foundationSoil.sgamma(Beff= Beff, Leff= Leff)
# Depth factors.
dq= foundationSoil.dq(D= D, Beff= Beff)
dgamma= foundationSoil.dgamma()
# Load inclination effect factors.
iq= foundationSoil.iq(Vload= V, HloadB= H, HloadL= 0.0)
igamma= foundationSoil.igamma(Vload= V, HloadB= H, HloadL= 0.0, Beff= Beff, Leff= Leff)
# Ultimate bearing capacity.
q= foundationSoil.gamma()*D
# qu_ref= Nq*sq*dq*iq*q+0.5*foundationSoil.gamma()*Beff*Ngamma*sgamma*dgamma*igamma
qu_ref= 355.0304585210153e3
qu= foundationSoil.qu(q= q, D= D, Beff= Beff, Leff= Leff, Vload= V, HloadB= H, HloadL= 0.0, NgammaCoef= 1.5)
err= abs(qu_ref-qu)/qu_ref

'''
print('Bearing capacity factors:')
print('  Nq= ', Nq)
print('  Ngamma= ', Ngamma)
print('Shape factors: ')
print('  sq= ', sq)
print('  sgamma= ', sgamma)
print('Depth factors: ')
print('  dq= ', dq)
print('  dgamma= ', dgamma)
print('Load inclination effect factors: ')
print('  iq= ', iq)
print('  igamma= ', igamma)
print('Ultimate bearing capacity: ')
print('  effective load q= ', q/1e3, 'kN')
print('  qu_ref= ', qu_ref/1e3, 'kN')
print('  q= ', q/1e3, 'kN/m2')
print('  D= ', D, 'm')
print('  B= ', Beff, 'm')
print('  L= ', Leff, 'm')
print('  V= ', V/1e3, 'kN')
print('  H= ', H/1e3, 'kN')
print('  qu= ', qu/1e3, 'kN')
print('  err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
