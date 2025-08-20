# -*- coding: utf-8 -*-
'''Check bolt axile strength calculation according to EN 1993-1-8:2005.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials

# Bolt
bolt= EC3_materials.BoltFastener(diameter= 20e-3, steelType= EC3_materials.bolt10dot9Steel)

boltTensionStrength= bolt.getDesignTensionStrength()
refValue= 0.9*1000*245/1.25
ratio1= abs(boltTensionStrength-refValue)/refValue

'''
print('bolt tension strength: Ftrd= ', boltTensionStrength/1e3, 'kN')
print(ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
