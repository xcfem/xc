# -*- coding: utf-8 -*-
''' Pile-wall displacement estimation according to the article "El Coeficiente
de Balasto en el Cálculo de Pantallas." of Fernando Muzás Labad. ETSAM UPM.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from geotechnics import earth_pressure
from geotechnics.earth_retaining import sheet_pile
    
# Soil model.
phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000)
rankineSoil.E= 50e6 # Deformation modulus (N/m2).

# Problem geometry.
H= 3.6 # Excavation depth (m).
t= 3.6 # Depth of penetration (m).

# Pile wall displacements.
Umax, Umin= sheet_pile.get_pile_wall_displacements_muzas(soil= rankineSoil, wallHeight= H+t, excavationDepth= H)

ratio1= abs(Umax-14.01902227240521e-3)/14.01902227240521e-3
ratio2= abs(Umin+0.1664962021138603e-3)/0.1664962021138603e-3

'''
print('internal friction angle: ', math.degrees(phi))
print('displacement at the top: Umax= ', Umax*1e3, ' mm')
print('displacement at the bottom: Umin= ', Umin*1e3, ' mm')
print(ratio1)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
