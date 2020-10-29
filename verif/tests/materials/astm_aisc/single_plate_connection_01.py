# -*- coding: utf-8 -*-
''' Verification of single plate shear and block shear strengths.
Home made test.
'''
from __future__ import division
from __future__ import print_function

import xc_base
import geom
from materials.astm_aisc import ASTM_materials

bolt= ASTM_materials.BoltFastener(20e-3, steelType= ASTM_materials.A490)
boltArray= ASTM_materials.BoltArray(bolt, nRows= 2, nCols= 1, dist= 80e-3)
designHoleDiameter= boltArray.bolt.getDesignHoleDiameter()
plateWidth= 0.14
plateLength= 0.1
eccentr= 5e-3
finPlate= ASTM_materials.FinPlate(boltArray, width= plateWidth, length= plateLength, thickness= 10e-3, steelType= ASTM_materials.A36, eccentricity= geom.Vector2d(eccentr,0.0))

Rd= 68e3
shearVector= geom.Vector2d(0.0,-Rd)

# Plate check
## Gross area subjected to shear
Agv= finPlate.getGrossAreaSubjectedToShear(shearVector)
ratio1= abs(Agv-1400e-6)/1400e-6
Anv= finPlate.getNetAreaSubjectedToShear(Agv,shearVector)
ratio2= abs(Anv-920e-6)/920e-6

## Shear strength
shearYielding= finPlate.getDesignShearYieldingStrength(shearVector)
shearYieldingRef= 189e3/0.9
ratio3= abs(shearYielding-shearYieldingRef)/shearYieldingRef
shearRupture= finPlate.getDesignShearRuptureStrength(shearVector)
shearRuptureRef= 166e3
ratio4= abs(shearRupture-shearRuptureRef)/shearRuptureRef
plateShearStrength= finPlate.getDesignShearStrength(shearVector)

## Block shear strength
blockAnt= finPlate.getNetAreaSubjectedToBlockTension(shearVector)
blockAntRef= ((plateLength-designHoleDiameter)/2.0-eccentr)*finPlate.thickness
ratio5= abs(blockAnt-blockAntRef)/blockAntRef
blockAgv= finPlate.getGrossAreaSubjectedToBlockShear(shearVector)
blockAgvRef=(plateWidth-30e-3)*finPlate.thickness
ratio6= abs(blockAgv-blockAgvRef)/blockAgvRef
blockAnv= finPlate.getNetAreaSubjectedToBlockShear(shearVector)
blockAnvRef= (plateWidth-30e-3-1.5*designHoleDiameter)*finPlate.thickness
ratio7= abs(blockAnv-blockAnvRef)/blockAnvRef
plateDesignBlockShearStrength= finPlate.getDesignBlockShearStrength(shearVector, Ubs= 1.0)
plateDesignBlockShearStrengthRef= 223e3
ratio8= abs(plateDesignBlockShearStrength-plateDesignBlockShearStrengthRef)/plateDesignBlockShearStrengthRef

'''
print('Rd= ', Rd/1e3, 'kN')
print('Agv= ', Agv*1e6, ' mm2')
print('ratio1= ', ratio1)
print('Anv= ', Anv*1e6, ' mm2')
print('ratio2= ', ratio2)
print('shearYielding= ', shearYielding/1e3, ' kN')
print('ratio3= ', ratio3)
print('shearRupture= ', shearRupture/1e3, ' kN')
print('ratio4= ', ratio4)
print('plateShearStrength= ', plateShearStrength/1e3, ' kN')
print('blockAnt= ', blockAnt*1e6, ' mm2')
print('blockAntRef= ', blockAntRef*1e6, ' mm2')
print('ratio5= ', ratio5)
print('blockAgv= ', blockAgv*1e6, ' mm2')
print('blockAgvRef= ', blockAgvRef*1e6, ' mm2')
print('ratio6= ', ratio6)
print('blockAnv= ', blockAnv*1e6, ' mm2')
print('blockAnvRef= ', blockAnvRef*1e6, ' mm2')
print('ratio7= ', ratio7)
print('plateDesignBlockShearStrength= ', plateDesignBlockShearStrength/1e3, ' kN')
print('ratio8= ', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-12 and ratio2<1e-12 and ratio3<1e-12 and ratio4<1e-2 and ratio5<1e-12 and ratio6<1e-12 and ratio7<1e-12 and ratio8<.01):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
