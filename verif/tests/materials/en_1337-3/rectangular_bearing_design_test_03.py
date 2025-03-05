# -*- coding: utf-8 -*-
''' Test of reinforced rectangular elastomeric bearing design according
    to EN 1337-3:2005.

    Test bearing definition from the en_1337_bearings module.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from misc_utils import log_messages as lmsg
from materials.en_1337 import en_1337_bearings
import csv
    
# Maximum displacements from the Mageba report (page 1).
vxd= 141.8e-3 # m
vyd= 33.5e-3 # m
# Rotations from the Mageba report (page 1).
alpha_ad= 0.0030 # rad
alpha_bd= 0.0 # rad
# Loads from the Mageba report (page 1).
Fzd= 2746.8e3 # design vertical load.
Vmin= 1226.3e3 # minimum vertical load.
Vperm= 1589.2e3 # design permanent load.
Fxd= 166.8e3 # design horizontal load along x.
Fyd= 49.1e3 # design horizontal load along y.

## Elastomeric bearing.
# bearingCode: code of the bearing in the data dictionary.
# C: side cover.
# ted: edge cover.
# tso: thickness of outer steel reinforcing plate.
# G: (optional) nominal value of conventional shear modulus of elastomeric
#    bearing (see table 1 of EN 1337-3:2005).
# gammaM: (optional) partial safety factor which value may be chosen in the
#         National Annex. The recommended value is γm = 1.00.
# fy: (optional) yield stress of the reinforcing steel.
# bearing_type: (optional) string that identifies the type of the bearing in the#               problem.
keys= ['code', 'a', 'b', 'tb', 'ti' , 'ts', 'e']
units_conv_factors= [None, 1e3, 1e3, 1e3, 1e3, 1e3, 1e3]
rows= en_1337_bearings.get_rectangular_bearings_table(keys= keys, units_conv_factors= units_conv_factors)
# with open('en_1337_rectangular_bearings.csv', 'w') as f:
#     writer= csv.writer(f)
#     writer.writerows(rows)

nRows= len(rows)

# print(nRows)

testOK= (nRows==124)
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
