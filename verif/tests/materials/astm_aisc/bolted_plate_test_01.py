# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Steel bolts according to AISC 360-16 verification test.
    Based on the:
    Example – Design of a Bolted Tension Bearing-Type Connection
    of the course:
    Design of Bolts in Shear-BearingConnections per AISC LRFD 3rdEdition (2001)
'''

in2m= 25.4e-3
kip2N= 4448.2216

import math
from materials.astm_aisc import ASTM_materials



bolt= ASTM_materials.BoltFastener(0.75*in2m, group= 'A')
boltArray= ASTM_materials.BoltArray(bolt, nRows= 2, nCols= 2)
boltedPlate= ASTM_materials.BoltedPlate(boltArray, thickness= 20e-3)

Pd= 121.6*kip2N
CFShear= boltArray.getDesignShearEfficiency(Pd, doubleShear= True)
CFCenterDist= boltArray.checkDistanceBetweenCenters()
minPlateWidth= boltArray.getMinPlateWidth()
CFthickness= boltedPlate.checkThickness(Pd)
ratio1= abs(CFShear-0.85024866348)/0.85024866348
ratio2= abs(CFCenterDist-0.762)/0.762
ratio3= abs(CFthickness-0.835501616559)/0.835501616559

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
    print("test ",fname,": ok.")
else:
    lmsg.error(fname+' ERROR.')
