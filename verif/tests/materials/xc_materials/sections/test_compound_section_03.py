# -*- coding: utf-8 -*-
''' Compound section with holes verification test. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

#              ^ Y                    
#              |
#
#      +--------------+
#      |              |
#      |              |
#      |              |   -> Z
#      |              |
#      |      O       |
#      +--------------+


import math
import geom
from materials.sections import section_properties as sp

# Rectangular section with hole
mainSection= sp.RectangularSection(name= 'mainSection', b= 1.0, h= 1.0)
ductDiam= 97e-3
hole= sp.Hole(name= 'hole', section= sp.CircularSection(name= 'duct', Rext= ductDiam/2.0, Rint=0.0))

positions= [geom.Pos2d(0,0), geom.Pos2d(0,-.396)]
sectionList= list()
for p, section in zip(positions, [mainSection, hole]):
    sectionList.append([p, section])

compoundSection= sp.CompoundSection('compoundSection',sectionList, Iw= 0.0)

# Compute area.
ductArea= math.pi*(ductDiam/2.0)**2 # duct area.
refArea= 1-ductArea
A= compoundSection.A()
ratio1= abs(A-refArea)/refArea

# Compute centroid
yCenter= compoundSection.yCenterOfMass()
zCenter= compoundSection.zCenterOfMass()
ratio2= math.sqrt((yCenter-0.0029481515663009874)**2+zCenter**2)

# Compute yMax
yMax= compoundSection.yMax()
refYMax= 0.5+yCenter
ratio3= abs(yMax-refYMax)/refYMax
# Compute hCOG (distance from the bottom fiber to the center of
# gravity of the section).
hCOG= compoundSection.hCOG()
refHCOG= refYMax
ratio4= abs(hCOG-refHCOG)/refHCOG

# Section modulus.
Wz= compoundSection.Wzel()
refWz= 0.16335982026435222
ratio5= abs(Wz-refWz)/refWz

'''
print('A= ', A, 'refA= ', refArea, 'ratio1= ', ratio1)
print('yCenter= ', yCenter, 'zCenter= ', zCenter, 'ratio2= ', ratio2)
print('yMax= ', yMax, 'refYMax= ', refYMax, 'ratio3= ', ratio3)
print('hCOG= ', hCOG, 'refHCOG= ', refHCOG, 'ratio4= ', ratio4)
print('Wz= ', Wz, 'refWz= ', refWz, 'ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-8) and (abs(ratio2)<1e-8) and (abs(ratio3)<1e-8) and (abs(ratio4)<1e-8) and (abs(ratio5)<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
