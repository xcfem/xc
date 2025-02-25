# -*- coding: utf-8 -*-
''' Check rectangular hollow section class.

Based on the sectionproperties results given here:
https://www.eng-tips.com/threads/formula-for-warping-constant-of-box-section.455965/

See https://github.com/robbievanleeuwen/section-properties/tree/master
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import geom
from materials.sections import section_properties

width= 2*25.4e-3
depth= width
thickness= 25.4e-3/2.0

box= section_properties.RectangularHollowSection(name= 'test', b= width, h= depth, t= thickness)

# Check results
## Area
A= box.A()
spA= 1935.48e-6 # Value computed with sectionproperties
ratio1= abs(A-spA)/spA

## Inertia
boxIz= box.Iz()
spIz= 520289.282*(.001)**4
ratio2= abs(boxIz-spIz)/spIz

boxIy= box.Iy()
spIy= spIz
ratio3= abs(boxIy-spIy)/spIy

## Torsion constant
boxJ= box.getJTorsion()
spJ= 860072.306*(.001)**4
ratio4= abs(boxJ-spJ)/spJ

## Warping constant
boxCw= box.getWarpingConstant()

'''
print('A= ', A)
print("ratio1= ",ratio1)
print('box Iy= ', boxIy)
print('sectionproperties Iy= ', spIy)
print("ratio3= ",ratio3)
print('box Iz= ', boxIz)
print('sectionproperties Iz= ', spIz)
print("ratio2= ",ratio2)
print("box J= ",boxJ)
print('thickness ratio: ', box.getThicknessRatio())
print('sectionproperties J= ', spJ)
print("ratio4= ",ratio4)
print("box Cw= ",boxCw)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10) and (abs(ratio4)<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
