# -*- coding: utf-8 -*-
''' Compute the reduction factor for the longitudinal wind force coefficient
    according to clause 8.3.4(1) of the Spanish Annex to UNE-EN 1991-1-4.

    The results are compared with those in the article: «Algunos problemas de cálculo específicos de los puentes para ferrocarril de alta velocidad» Miguel Ángel Astiz Suárez. Revista de Obras Públicas. June 2004. number: 3445.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from actions.wind import ec1_wind

# Compute the value of the force coefficient without free-end flow.
terrainCategory= 'II'
z= 20.0 # Deck height.
L= 1000.0 # Bridge length.

# Compute reduction factor.
## Compute turbulent length scale.
Lz= ec1_wind.get_turbulent_length_scale(terrainCategory= terrainCategory, z= z)
## Compute phi function.
phi_z= ec1_wind.get_phi_long_wind_reduction_factor(terrainCategory= terrainCategory, z= z, L=L)
## Reduction factor.
C1= ec1_wind.get_spanish_annex_long_wind_reduction_factor(terrainCategory= terrainCategory, z= z, L=L, c0= 1.0)
ratio= abs(C1-0.6405406122185486)/0.6405406122185486
# The article gives a value of C1= 0.69 but uses a different expression for C1
# (see equation 7 in th article).

'''
print(Lz)
print(phi_z)
print(C1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
