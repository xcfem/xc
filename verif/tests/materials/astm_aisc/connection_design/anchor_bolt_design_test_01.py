# -*- coding: utf-8 -*-
''' Test based on "Example A4—Single bolt, combined tension and shear"
    of the document:

    ACI 349.2R-07
    Guide to the Concrete Capacity Design (CCD) Method —
    Embedment Design Examples. American Concrete Institute.
    November 2007.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.astm_aisc import ASTM_materials

in2m= 25.4e-3
kip2N= 4448.2216
psi2Pa= 6894.757

# Concrete
fc= 4000*psi2Pa

# Loads
Nua= 40*kip2N # Factored axial load.

# Bolt
AB105= ASTM_materials.F1554gr105
anchor= ASTM_materials.AnchorBolt(name= 'ExampleA4', steel= AB105, diameter=1.125*in2m)
h_ef= 17.57*in2m

d= anchor.diameter
Ase=  anchor.getTensionEffectiveCrossSectionalArea()
ratio1= abs(Ase-492.433431069e-6)/492.433431069e-6
Nsa= anchor.getNominalTensileStrength()
ratio2= abs(Nsa-424.477617581e3)/424.477617581e3
# Concrete breakout strenght calculated using
# equation (D-8) instead of (D-7) see note
# in the example.
Ncb= anchor.getNominalConcreteBreakoutStrength(h_ef, fc)
ratio3= abs(Ncb-534.510669747e3)/534.510669747
# Concrete pullout strength.
Np= anchor.getNominalPulloutStrength(fc)
ductilityCF= anchor.checkDuctility(h_ef, fc)


plateWasher=  ASTM_materials.SquarePlateWasher(anchor, side= 2.25*in2m, thickness= 3/8*in2m)
anchor.plateWasher= plateWasher
ductilityCF= anchor.checkDuctility(h_ef, fc)
Np2= anchor.getNominalPulloutStrength(fc)
ratio4= abs(Np2-579.120061024e3)/579.120061024

'''
print('d= ', d*1e3, 'mm', d/in2m, ' in')
print('Ase= ', Ase*1e6, 'mm2', Ase/in2m/in2m, ' in2')
print('ratio1= ', ratio1)
print('fu= ', AB105.fu/1e6, 'MPa', AB105.fu/psi2Pa/1000, ' ksi')
print('Nsa= ', Nsa/1e3, ' kN', Nsa/kip2N, ' kips')
print('ratio2= ', ratio2)
print('Ncb= ', Ncb/1e3, ' kN', Ncb/kip2N, ' kips')
print('ratio3= ', ratio3)
print('Np= ', Np/1e3, ' kN', Np/kip2N, ' kips')
print('ductilityCF= ', ductilityCF)
print('Np2= ', Np2/1e3, ' kN', Np2/kip2N, ' kips')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6 and ratio4<1e-6):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
