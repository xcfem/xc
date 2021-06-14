# -*- coding: utf-8 -*-
''' Shear strength according to clause 58.6 of EAE. Home made test'''

from materials.eae import EAE_materials

bolt= EAE_materials.BoltFastener(name= '8.8bolt', steel= EAE_materials.iso898_8_8, diameter= 22e-3)

FvRd= bolt.getDesignShearStrength(threadsExcluded= True)
ratio1= abs(FvRd-145.97096105639613e3)/145.97096105639613e3
FtRd= bolt.getDesignTensileStrength()
ratio2= abs(FtRd-174.528e3)/174.528e3

numberOfShearAnchors= 6
numberOfTensionAnchors= 4
anchorsLeverArm= 0.3
F= 225e3
M= F*1.0
Qd= F/numberOfShearAnchors
Td= M/anchorsLeverArm/numberOfTensionAnchors
eff= bolt.getShearTensileEfficiency(Qd,Td,threadsExcluded= True)
ratio3= abs(eff-1.0242762564846506)/1.0242762564846506

'''
print('FvRd= ', FvRd/1e3, 'kN')
print('ratio1= ', ratio1)
print('FtRd= ', FtRd/1e3, 'kN')
print('ratio2= ', ratio2)
print('Qd= ', Qd/1e3, 'kN')
print('Qd/FvRd= ', Qd/FvRd)
print('Td= ', Td/1e3, 'kN')
print('Td/FtRd= ', Td/1.4/FtRd)
print('eff= ', eff)
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) & (ratio2<1e-6) & (ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
