# -*- coding: utf-8 -*-
''' Home made test.'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import xc_base
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
chsSection= EC3_materials.CHSShape(S355JR,'CHS_48.3_5.0') 

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

compoundSection= sp.CompoundSection('CDTrager',sectionList)
fs2dSectionShape= compoundSection.defElasticShearSection2d(preprocessor,S355JR)

yCenter= compoundSection.yCenterOfMass()
zCenter= compoundSection.zCenterOfMass()
alphaY= compoundSection.alphaY()
ratio1= (alphaY-0.5)/0.5
ratio2= compoundSection.A()-8.0*chsSection.A()
ratio3= (compoundSection.Iz()-0.001361296)/0.001361296
ratio4= (compoundSection.Iy()-0.000708496)/0.000708496


'''
print yCenter, zCenter
print 'compound A= ', compoundSection.A()
print 'ratio2= ', ratio2
print 'compound Iz= ', compoundSection.Iz()
print 'ratio3= ', ratio3
print 'compound Iy= ', compoundSection.Iy()
print 'ratio4= ', ratio4
print compoundSection.alphaY()
'''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(abs(yCenter)<1e-15 and abs(zCenter)<1e-15 and abs(ratio1)<1e-15 and abs(ratio2)<1e-15and abs(ratio3)<1e-15 and abs(ratio4)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
