# -*- coding: utf-8 -*-
''' Test based on example II.A-17 SINGLE-PLATE CONNECTION (CONVENTIONAL
 – BEAM-TO-COLUMN FLANGE) from the document: Design examples version 14.0.
American Institute of Steel Construction. 2011
'''
from __future__ import division
from __future__ import print_function


in2m= 25.4e-3
kip2N= 4448.2216

import xc_base
import geom
from materials.astm_aisc import ASTM_materials

bolt= ASTM_materials.BoltFastener(0.75*in2m, steelType= ASTM_materials.A325) # group A
boltArray= ASTM_materials.BoltArray(bolt, nRows= 4, nCols= 1, dist= 3*in2m)
eccentr= (4.5/2.0-1.25)*in2m
finPlate= ASTM_materials.FinPlate(boltArray, width= 11.5*in2m, length= 4.5*in2m, thickness= 0.25*in2m, steelType= ASTM_materials.A36, eccentricity= geom.Vector2d(eccentr,0.0))

Rd= 49.6*kip2N

## Check bolt strength
CFShear= boltArray.getDesignShearEfficiency(Rd, doubleShear= False)
loadDir= geom.Vector2d(0.0,-1.0)
CF= finPlate.getDesignEfficiency(geom.Vector2d(0.0,-Rd), Ubs= 1.0)
CFRef= 49.6/52.2
ratio1= abs(CF-CFRef)/CFRef

'''
print('Rd= ', Rd/1e3, 'kN')
print('CF(Shear)= ', CFShear)
print('CF= ', CF)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<.05):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

quit()
CFCenterDist= boltArray.checkDistanceBetweenCenters()
minPlateWidth= boltArray.getMinPlateWidth()
CFthickness= finPlate.checkThickness(Pd)
ratio1= abs(CFShear-0.85024866348)/0.85024866348
ratio2= abs(CFCenterDist-0.762)/0.762
ratio3= abs(CFthickness-0.835501616559)/0.835501616559

'''
print('dist. : ', boltArray.dist*1e3, 'mm')
print('CF(center dist.)= ', CFCenterDist)
print('min. plate width: ', minPlateWidth*1e3, 'mm')
print('plate dimensions: ', finPlate.getPlateDimensions(), 'm')
print('min. thickness: ', finPlate.getMinThickness(Pd)*1e3, 'mm')
print('CF(thickness.)= ', CFthickness)
'''

