# -*- coding: utf-8 -*-
'''Check bolt distances according to table 3.3 of EN 1993-1-8:2005.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials

# Bolt
bolt= EC3_materials.BoltFastener(diameter= 20e-3, steelType= EC3_materials.bolt10dot9Steel)

boltSpacing= bolt.getMinDistanceBetweenCenters(oversized= False, parallelToLoad= False)
d0= 20e-3+2e-3
boltSpacingRef= 2.4*d0
ratio1= abs(boltSpacing-boltSpacingRef)/boltSpacingRef
distToEdge= bolt.getMinimumEdgeDistance(oversized= False, slotted= False)
distToEdgeRef= 1.2*d0
ratio1= abs(distToEdge-distToEdgeRef)/distToEdgeRef

'''
print('Bolt spacing: ', boltSpacing*1e3, 'mm')
print(ratio1)
print('Distance to edge: ', distToEdge*1e3, 'mm')
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-3 and abs(ratio2)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
