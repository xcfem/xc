# -*- coding: utf-8 -*-
''' Check rectangular hollow section class.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import geom
from materials.sections import section_properties as sp

width= 4.0
depth= 2.0
thickness= 0.35

# Define equivalent compound section to compare results.
topWall= sp.RectangularSection('topWall', b= width, h= thickness)
topWallCenter= (0.0, (depth-thickness)/2.0)
bottomWall= sp.RectangularSection('bottomWall', b= width, h= thickness)
bottomWallCenter= (0.0, -topWallCenter[1])
leftWall= sp.RectangularSection('leftWall', b= thickness, h= depth-2*thickness)
leftWallCenter= (-(width-thickness)/2.0, 0.0)
rightWall= sp.RectangularSection('rightWall', b= thickness, h= depth-2*thickness)
rightWallCenter= (-leftWallCenter[0], 0.0)
positions= [topWallCenter, bottomWallCenter, leftWallCenter, rightWallCenter]
sections= [topWall, bottomWall, leftWall, rightWall]
sectionList= []
for p, sct in zip(positions, sections):
    sectionList.append([geom.Pos2d(p[0],p[1]),sct])

compoundSection= sp.CompoundSection('compoundSection',sectionList, Iw= 0.0)


box= sp.RectangularHollowSection(name= 'test', b= width, h= depth, t= thickness)

# Check results
## Area
A= box.A()
Aref= (width*2)-(width-0.7)*(depth-0.7)
ratio1= abs(A-Aref)/Aref

## Inertia
boxIz= box.Iz()
cmpIz= compoundSection.Iz()
ratio2= abs(boxIz-cmpIz)/cmpIz

boxIy= box.Iy()
cmpIy= compoundSection.Iy()
ratio3= abs(boxIy-cmpIy)/cmpIy

boxJ= box.getJTorsion()

'''
print('A= ', A)
print("ratio1= ",ratio1)
print('box Iy= ', boxIy)
print('compound section Iy= ', cmpIy)
print("ratio3= ",ratio3)
print('box Iz= ', boxIz)
print('compound section Iz= ', cmpIz)
print("ratio2= ",ratio2)
print("box J= ",boxJ)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
