# -*- coding: utf-8 -*-
''' Verification test based on example 1 of section 6-8
    of TM 5-809-3 manual.
'''
    
from __future__ import division
from __future__ import print_function

import math
from materials.aci import ACI_materials
from materials.tm5_809_3 import tm5_809_3_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

inch2Meter= .0254
foot2Meter= .3048
pound2Newton= 4.4482216282509
lbSqft2Pascal= 47.880258888889


S_iu= 24.0 # Cell spacing (inches).
S= S_iu*inch2Meter # Cell spacing (meters).
cellReinf= tm5_809_3_materials.CMUWallCellReinforcement(reinfArea= ACI_materials.num6Area)
cmuWall= tm5_809_3_materials.CMUWallFabric(thickness= 12*inch2Meter,spacing= S, cellReinf= cellReinf)
wallHeight= 24*foot2Meter # 24 ft-> m

ratios= list()
n= cmuWall.get_n()
ratios.append(abs(n-21.5)/21.5)

mMax= 2182*4.44822 # N m/m Design moment

Fm= cmuWall.mortar.Fm()
FmIU= Fm/pound2Newton*inch2Meter**2 # Imperial units.
ratios.append(abs(FmIU-445.499979756)/445.499979756)
p= cmuWall.getSteelRatio()
ratios.append(abs(p-0.0032)/0.0032)
k= cmuWall.getKCoefficient()
ratios.append(abs(k-0.308)/0.308)
b= cmuWall.getEffectiveWidth()
ratios.append(abs(b/inch2Meter-24)/24)
j= cmuWall.getJCoefficient()
ratios.append(abs(j-0.897)/0.897)

# Masonry resisting moment.
Mrm= cmuWall.getMasonryResistingMomentPerUnitLength()
MrmIU= Mrm/pound2Newton/foot2Meter*inch2Meter # Imperial units
MrmIU_ref= 4147/S_iu
ratios.append(abs(MrmIU-MrmIU_ref)/MrmIU_ref)

# Reinforcing steel resisting moment.
Mrs= cmuWall.getReinforcementResistingMomentPerUnitLength()
MrsIU= Mrs/pound2Newton/foot2Meter*inch2Meter # Imperial units
MrsIU_ref= 4586.1816/S_iu
ratios.append(abs(MrsIU-MrsIU_ref)/MrsIU_ref)

# Allowable axial stress.
Fa= cmuWall.getAllowableAxialStress(wallHeight)
FaIU= Fa/pound2Newton*inch2Meter**2
FaIU_ref= 211.68
ratios.append(abs(FaIU-FaIU_ref)/FaIU_ref)

# Capacity factor
effectiveArea= cmuWall.getEffectiveArea()
effectiveAreaIU= cmuWall.getEffectiveArea()/inch2Meter**2
Mu= 1.33*cmuWall.getResistingMomentPerUnitLength()
Fu= 1.33*cmuWall.getResistingAxialForcePerUnitLength(wallHeight)
axialLoad= 38.3*pound2Newton/(inch2Meter**2)*cmuWall.getEffectiveAreaPerUnitLength()

capacityFactor= cmuWall.getCapacityFactor(axialLoad,mMax,wallHeight)/1.33
refCapacityFactor= 0.14+0.79
ratios.append(abs(capacityFactor-refCapacityFactor)/refCapacityFactor)

'''
print('Fm= ', Fm)
print('FmIU= ', FmIU)
print('n= ', n)
print('Mmax= ', mMax/1e3, 'kN.m/m')
print('p= ', p)
print('k= ', k)
print('b= ', b)
print('j= ', j)
print('Mrm= ', Mrm/1e3, 'kN.m/m')
print('MrmIU= ', MrmIU, 'lb.ft/ft')
print('MrmIU_ref= ', MrmIU_ref, 'lb.ft/ft')
print('Mrs= ', Mrs/1e3, 'kN.m/m')
print('MrsIU= ', MrsIU, 'lb.ft/ft')
print('MrsIU_ref= ', MrsIU_ref, 'lb.ft/ft')
print('Fa= ', Fa/1e6, 'MPa')
print('FaIU= ', FaIU, 'lb/in2')
print('effective area: ', effectiveArea,' m2')
print('effective area IU: ', effectiveAreaIU,' in2')
print('Mu= ', Mu/1e3, 'kN.m/m')
print('Fu= ', Fu/1e3, 'kN/m')
print('axialLoad= ', axialLoad/1e3, 'kN/m')
print('capacityFactor= ', capacityFactor)
print(ratios)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(max(ratios))<0.05 and abs(min(ratios))<0.05):
  print('test '+fname+': ok.')
else:
  print(ratios)
  lmsg.error(fname+' ERROR.')
