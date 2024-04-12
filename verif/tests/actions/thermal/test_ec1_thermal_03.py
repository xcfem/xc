# -*- coding: utf-8 -*-
''' Test based on example 35-S Characteristic thermal actions in bridges ‐ Temperature difference components of the publication:

EN 1990, EN 1991 - Eurocodes 0-1 - Worked Examples. Carlo Sigmund. 2014. ISBN n.: 978-1-291-84215-9. https://eurocodespreadsheets.jimdo.com/tech-docs-en/
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from actions.thermal import EC1_thermal

# Temperature difference components.
surfaceThickness= 100e-3

kSurHeat= EC1_thermal.getKSur(bridgeType= EC1_thermal.steel_deck_type, topWarmerThanBottom= True, surfacingDepth= surfaceThickness, ballast= False, waterproofedOnly= False)
kSurCool= EC1_thermal.getKSur(bridgeType= EC1_thermal.steel_deck_type, topWarmerThanBottom= False, surfacingDepth= surfaceThickness, ballast= False, waterproofedOnly= False)

DeltaTmHeat= EC1_thermal.getLinearTemperatureDifferenceComponent(bridgeType= EC1_thermal.steel_deck_type, topWarmerThanBottom= True, surfacingDepth= surfaceThickness, ballast= False, waterproofedOnly= False)
DeltaTmCool= EC1_thermal.getLinearTemperatureDifferenceComponent(bridgeType= EC1_thermal.steel_deck_type, topWarmerThanBottom= False, surfacingDepth= surfaceThickness, ballast= False, waterproofedOnly= False)

# Check results.
err= (kSurHeat-0.7)**2+(kSurCool-1.2)**2
err+= (DeltaTmHeat-12.6)**2+(DeltaTmCool-15.6)**2
err= math.sqrt(err)

# print(kSurHeat, kSurCool)
# print(DeltaTmHeat, DeltaTmCool)
# print(err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
