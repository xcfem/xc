# -*- coding: utf-8 -*-
''' Shear strength according to clause 58.6 of EAE. Home made test'''

from materials.eae import EAE_materials

bolt= EAE_materials.BoltFastener(name= '8.8bolt', steel= EAE_materials.iso898_8_8, diameter= 22e-3)

FvRd= bolt.getDesignShearStrength(threadsExcluded= True)
ratio1= abs(FvRd-145.97096105639613e3)/145.97096105639613e3
FtRd= bolt.getDesignTensileStrength()
ratio2= abs(FtRd-174.528e3)/174.528e3

'''
print('FvRd= ', FvRd/1e3, 'kN')
print('ratio1= ', ratio1)
print('FtRd= ', FtRd/1e3, 'kN')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) & (ratio2<1e-5):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
