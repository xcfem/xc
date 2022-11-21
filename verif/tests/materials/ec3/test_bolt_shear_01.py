# -*- coding: utf-8 -*-
'''Classification of steel cross-sections (clause 5.5 of EN 1993-1-1:2005).'''

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials

# Bolt
bolt= EC3_materials.BoltFastener(diameter= 20e-3, steelType= EC3_materials.bolt8dot8Steel)

boltArea= bolt.getArea()
boltTensileStressArea= bolt.getTensileStressArea()
ratio1= abs(boltTensileStressArea-244.794391734083e-6)/244.794391734083e-6

boltShearStrength= bolt.getDesignShearStrength(numberOfShearPlanes= 2)
ratio2= abs(boltShearStrength-188.00209285177576e3)/188.00209285177576e3

boltShearStrengthTE= bolt.getDesignShearStrength(numberOfShearPlanes= 2, threadsExcluded= True)
ratio3= abs(boltShearStrengthTE-241.2743157956961e3)/241.2743157956961e3

'''
print('bolt area: A= ', boltArea*1e6, 'mm2')
print('bolt tensile stress area: A= ', boltTensileStressArea*1e6, 'mm2')
print(ratio1)
print('bolt shear strength: Fvrd= ', boltShearStrength/1e3, 'kN')
print(ratio2)
print('bolt shear strength (threads excluded): Fvrd= ', boltShearStrengthTE/1e3, 'kN')
print(ratio3)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
