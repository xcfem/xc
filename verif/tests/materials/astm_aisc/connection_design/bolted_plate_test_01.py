# -*- coding: utf-8 -*-
''' Steel bolts according to AISC 360-16 verification test.
    Based on the:
    Example – Design of a Bolted Tension Bearing-Type Connection
    of the course:
    Design of Bolts in Shear-BearingConnections per AISC LRFD 3rdEdition (2001)
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"


import math
from materials.astm_aisc import ASTM_materials
from misc_utils import units_utils

bolt= ASTM_materials.BoltFastener(0.75*units_utils.inchToMeter, steelType= ASTM_materials.A325) # group A
boltArray= ASTM_materials.BoltArray(bolt, nRows= 2, nCols= 2)
boltedPlate= ASTM_materials.BoltedPlate(boltArray, thickness= 20e-3, steelType= ASTM_materials.A36)

Pd= 121.6*units_utils.kipToN
CFShear= boltArray.getDesignShearEfficiency(Pd, doubleShear= True)
CFCenterDist= boltArray.checkDistanceBetweenCenters()
CFthickness= boltedPlate.checkThickness(Pd)
minPlateWidth= boltedPlate.getMinWidth()
ratio1= abs(CFShear-0.85024866348)/0.85024866348
ratio2= abs(CFCenterDist-0.762)/0.762
ratio3= abs(CFthickness-0.867667222586)/0.867667222586

'''
print('Pd= ', Pd/1e3, 'kN')
print('CF(Shear)= ', CFShear)
print('dist. : ', boltArray.dist*1e3, 'mm')
print('CF(center dist.)= ', CFCenterDist)
print('min. plate width: ', minPlateWidth*1e3, 'mm')
print('plate dimensions: ', boltedPlate.getPlateDimensions(), 'm')
print('min. thickness: ', boltedPlate.getMinThickness(Pd)*1e3, 'mm')
print('CF(thickness.)= ', CFthickness)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-6 and ratio2<1e-6 and ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
