# -*- coding: utf-8 -*-
''' Test based on design example 4.2 of the book Handbook of Structural
Steel Connection Design and Details. Akbar R. Tamboli, P.E. F. ASCE Editor
McGraw-Hill Education.'''

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Testing hinge development in a cantilever.

from materials.astm_aisc import ASTM_materials
from materials.sections.structural_shapes import aisc_metric_shapes

#Materials
## Steel material
steel= ASTM_materials.A572
steel.gammaM= 1.00
## Profile geometry (the example on the book says W690X170 but uses
## the values of an W690X140).
shape= ASTM_materials.WShape(steel,aisc_metric_shapes.getUSLabel('W690X140'))

## Probable plastic moment at plastic hinge according
## to clause 2.4.3 of AISC-358.
Mpr= shape.getProbableMaxMomentAtPlasticHinge(majorAxis= True)
ratio1= abs(Mpr-1989e3)/1989e3

## Force in the flanges (conservative value).
leverArm= shape.h()
F_flange= Mpr/leverArm
ratio2= abs(F_flange-2908e3)/2908e3

## Maximum bolt diameter.
db= shape.getFlangeMaximumBoltDiameter()
tmpBolt= ASTM_materials.getBoltForHole(db)
ratio3= (tmpBolt.getName()=='M24')
bolt=  ASTM_materials.BoltFastener(tmpBolt.diameter, steelType= ASTM_materials.A490)

## Estimate number of bolts.
N= bolt.getNumberOfBoltsForShear(1.25*F_flange, numberOfRows= 2, threadsExcluded= True)

nRows= 2
nCols= N/nRows

boltArray= ASTM_materials.BoltArray(bolt, nRows, nCols, dist= None)
flangeAsBoltedPlate= ASTM_materials.BoltedPlate(boltArray, width= shape.getFlangeWidth(), thickness= shape.getFlangeThickness(), steelType= shape.steelType)

## Gross section yielding load and net section fracture load
flangeNetSectionFractureLoad= flangeAsBoltedPlate.getNetSectionFractureLoad()
flangeGrossSectionYieldingLoad= flangeAsBoltedPlate.getGrossSectionYieldingLoad()
ratio4= abs(flangeGrossSectionYieldingLoad-1656.207e3)/1656.207e3+abs(flangeNetSectionFractureLoad-1666.98e3)/1666.98e3

## Check local buckling
lambda_hd_flange= shape.getLambdaHDFlange()
flange_buckling_check= shape.flangeLocalBucklingCheck(highlyDuctile= True)
lambda_hd_web= shape.getLambdaHDWeb(N= 0.0, M= Mpr)
web_buckling_check= shape.webLocalBucklingCheck(N= 0.0, M= Mpr,highlyDuctile= True)
ratio5= abs(lambda_hd_flange-7.35)/7.35
ratio6= abs(lambda_hd_web-59.0)/59.0

## Bolted plate.
boltedPlate= ASTM_materials.BoltedPlate(boltArray, thickness= 28e-3, width= 360e-3, steelType= shape.steelType)
leverArm= shape.h()+boltedPlate.thickness
F_plate= 2339e3/leverArm
### Check number of bolts for the plate.
Nplate= bolt.getNumberOfBoltsForShear(F_plate, numberOfRows= 2, threadsExcluded= True)
ratio7= (Nplate<N)
### Check net and gross areas for the plate
plateNetSectionFractureLoad= boltedPlate.getNetSectionFractureLoad()
plateGrossSectionYieldingLoad= boltedPlate.getGrossSectionYieldingLoad()
ratio8= abs(plateGrossSectionYieldingLoad-3477.6e3)/3477.6e3+abs(plateNetSectionFractureLoad-3805.2e3)/3805.2e3

'''
print('Mpr= ', Mpr/1e3, 'kN.m')
print('ratio1= ', ratio1)
print('lever arm: ', leverArm, ' m')
print('F_flange= ', F_flange/1e3, 'kN')
print('ratio2= ', ratio2)
print('maximum hole diameter db= ', db*1e3, ' mm')
print('maximum bolt diameter: ', bolt.diameter*1e3, ' mm')
print('ratio3= ', ratio3)
print('number of bolts N= ', N)
print('flange gross section yielding load: ', flangeGrossSectionYieldingLoad/1e3, ' kN')
print('flange net section fracture load: ', flangeNetSectionFractureLoad/1e3, ' kN')
print('ratio4= ', ratio4)
print('lambda_hd flange: ', lambda_hd_flange)
print('flange buckling check: ', flange_buckling_check)
print('ratio5= ', ratio5)
print('lambda_hd web: ', lambda_hd_web)
print('web buckling check: ', web_buckling_check)
print('ratio6= ', ratio6)
print('F_plate= ', F_plate/1e3, 'kN')
print('Nplate= ', Nplate)
print('ratio7= ', ratio7)
print('plate gross section yielding load: ', plateGrossSectionYieldingLoad/1e3, ' kN')
print('plate net section fracture load: ', plateNetSectionFractureLoad/1e3, ' kN')
print('ratio8= ', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-2 and ratio2<1e-2 and ratio3 and ratio4<1e-3 and ratio5<.05 and ratio6<0.05 and ratio7 and ratio8<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
