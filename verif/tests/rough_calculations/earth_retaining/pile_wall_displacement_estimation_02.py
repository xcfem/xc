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
from scipy.constants import g
    
# Soil model.
phi= math.pi/6.0 # internal friction angle of the soil.
delta= 2/3.0*phi # friction angle between the soil and the back surface
                 # of the retaining wall.
gamma= 18e3
rho= gamma/g
rankineSoil= earth_pressure.RankineSoil(phi, rho= rho)
rankineSoil.E= 50e6 # Deformation modulus (N/m2).

# Problem geometry (see table 5 in page 11 of the article).
ti= [3.0, 3.5, 4.0] # Depth of penetration (m).
Li= [t+3.0 for t in ti]
Hi= list() # Excavation depth (m).
Gi= list() # Rotation
Uoi= list() # displacements at the pile top.
Kai= list()
Kpi= list()
Kri= list()
for t, L in zip(ti, Li):
    H= L-t
    Hi.append(H)
    Gi.append(sheet_pile.get_pile_wall_rotation_muzas(soil= rankineSoil, wallHeight= L, excavationDepth= H))
    Uoi.append(sheet_pile.get_pile_wall_initial_translation_muzas(soil= rankineSoil, wallHeight= L, excavationDepth= H))
    Kai.append(sheet_pile.get_unload_subgrade_reaction_from_rest_muzas(soil= rankineSoil, wallHeight= L, excavationDepth= H))
    Kri.append(sheet_pile.get_reload_subgrade_reaction_from_excavated_rest_muzas(soil= rankineSoil, wallHeight= L, excavationDepth= H))
    Kpi.append(sheet_pile.get_load_subgrade_reaction_from_rest_muzas(soil= rankineSoil, wallHeight= L, excavationDepth= H))

# The reference values have been computed with an spreadsheet, the results are
# slightly different (but close enough) from those of the article.
GiRef= [0.0018, 0.0009, 0.0005]
UoiRef= 3*[1.500012e-3]
KaiRef=	[3695346.8903962, 5147434.28089619, 6859200.47966908]
KriRef=	[17998560.1151908, 17998560.1151908, 17998560.1151908]
KpiRef=	[22172081.3423772, 30884605.6853772, 41155202.8780145]

err= 0.0
# Check G results.
for gi, gi_ref in zip(Gi, GiRef):
    err+= (gi-gi_ref)**2
# Check Uo results.
for uoi, uoi_ref in zip(Uoi, UoiRef):
    err+= (uoi-uoi_ref)**2
# Check Ka results.
for kai, kai_ref in zip(Kai, KaiRef):
    err+= (kai-kai_ref)**2
# Check Kr results.
for kri, kri_ref in zip(Kri, KriRef):
    err+= (kri-kri_ref)**2
# Check Kr results.
for kpi, kpi_ref in zip(Kpi, KpiRef):
    err+= (kpi-kpi_ref)**2

err= math.sqrt(err)

'''
print('ti= ', ti)
print('Hi= ', Hi)
print('Gi= ', Gi)
print('Uoi= ', Uoi)
print('Kai= ', Kai)
print('Kri= ', Kri)
print('Kpi= ', Kpi)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if err<1e-4:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
