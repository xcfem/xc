# -*- coding: utf-8 -*-
''' Compound section class verification test. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import geom
import xc
from materials.sections import section_properties as sp
from materials.ec3 import EC3_materials

# Problem type
mainBeam= xc.FEProblem()
mainBeam.title= 'Falsework support beams'
preprocessor= mainBeam.getPreprocessor   

#Materials
S355JR= EC3_materials.S355JR
S355JR.gammaM= 1.00
chsSection= EC3_materials.CHSShape(S355JR,'CHS_48.3_5') 

#              ^ Y                    
#              |
#
#     o       o o       o
#
#                          -> Z
#
#     o       o o       o
#

positions= [[-0.5,-0.5],[-0.5,-0.1],[-0.5,0.1],[-0.5,0.5],[0.5,-0.5],[0.5,-0.1],[0.5,0.1],[0.5,0.5]]
sectionList= []
for p in positions:
    sectionList.append([geom.Pos2d(p[0],p[1]),chsSection])

compoundSection= sp.CompoundSection('CDTrager',sectionList, Iw= 0.0)
fs2dSectionShape= compoundSection.defElasticShearSection2d(preprocessor,S355JR)

yCenter= compoundSection.yCenterOfMass()
zCenter= compoundSection.zCenterOfMass()
alphaY= compoundSection.alphaY()
ratio1= (alphaY-0.5)/0.5
ratio2= compoundSection.A()-len(positions)*chsSection.A()
refIy= 0.0013616018383254059 # 0.001361296
ratio3= (compoundSection.Iy()-refIy)/refIy
refIz= 0.0007086532212033031 # 0.000708496
ratio4= (compoundSection.Iz()-refIz)/refIz

'''
print(yCenter, zCenter)
print('ratio1= ', ratio1)
print('compound A= ', compoundSection.A())
print('ratio2= ', ratio2)
print('compound Iy= ', compoundSection.Iy())
print('ratio3= ', ratio3)
print('compound Iz= ', compoundSection.Iz())
print('ratio4= ', ratio4)
print(compoundSection.alphaY())
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(yCenter)<1e-15 and abs(zCenter)<1e-15 and abs(ratio1)<1e-15 and abs(ratio2)<1e-15 and abs(ratio3)<1e-15 and abs(ratio4)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

