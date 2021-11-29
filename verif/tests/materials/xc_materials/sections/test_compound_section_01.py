# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test.'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import xc_base
import geom
import xc
from materials.sections import section_properties as sp
from materials.ec3 import EC3_materials

# Geometry
girderSectionWidth= 0.21
girderSectionDepth= 1.60
distBetweenGirders= 4.0-2*0.6

girderSection= sp.RectangularSection('girderSection', b= girderSectionWidth, h= girderSectionDepth)

#              ^ Y                    
#              |
#
#     +-+             +-+
#     | |             | |
#     | |             | |
#     | |             | |   -> Z
#     | |             | |
#     | |             | |
#     +-+             +-+
#

positions= [[-distBetweenGirders/2.0,0.0],[distBetweenGirders/2.0,0.0]]
sectionList= []
for p in positions:
    sectionList.append([geom.Pos2d(p[0],p[1]),girderSection])

compoundSection= sp.CompoundSection('compoundSection',sectionList)

yCenter= compoundSection.yCenterOfMass()
zCenter= compoundSection.zCenterOfMass()

ratio1= compoundSection.A()-2*girderSectionWidth*girderSectionDepth
IzRef= 2*1/12.0*girderSectionWidth*girderSectionDepth**3
ratio2= (compoundSection.Iz()-IzRef)/IzRef
IyRef= 2*(1/12.0*girderSectionDepth*girderSectionWidth**3+girderSectionWidth*girderSectionDepth*(distBetweenGirders/2)**2)
ratio3= (compoundSection.Iy()-IyRef)/IyRef

'''
print(yCenter, zCenter)
print('compound A= ', compoundSection.A())
print('ratio1= ', ratio1)
print('compound Iz= ', compoundSection.Iz())
print('reference Iz= ', IzRef)
print('ratio2= ', ratio2)
print('compound Iy= ', compoundSection.Iy())
print('reference Iy= ', IyRef)
print('ratio3= ', ratio3)
print(compoundSection.alphaY())
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(yCenter)<1e-15 and abs(zCenter)<1e-15 and abs(ratio1)<1e-15 and abs(ratio2)<1e-15and abs(ratio3)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff.
# compoundSection.draw()
