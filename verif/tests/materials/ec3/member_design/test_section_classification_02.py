# -*- coding: utf-8 -*-
'''Classification of steel cross-sections (clause 5.5 of EN 1993-1-1:2005).

Inspired on the Example 6.6 of the SDC verifier.
https://sdcverifier.com/benchmarks/eurocode-3-example-6-6-cross-section-resistance-under-combined-bending-and-compression/
'''

from __future__ import print_function
from __future__ import division


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

from materials.ec3 import EC3_materials

steelMaterial= EC3_materials.S275JR

steelShape= EC3_materials.UBShape(steel=steelMaterial,name='UB457x191x98')
c1_1= steelShape.getClassInternalPartInCompression()
c1_2= steelShape.getClassInternalPartInBending()
c1_3= steelShape.getClassOutstandPartInCompression()
c= steelShape.getClassInCompression()
ratio1= abs(c1_3-1)
ratio2= abs(c1_1-3)
ratio3= abs(c-3)

'''
print('Oustand flanges classification under compression: ', c1_3)
print('Web classification under compression: ', c1_1)
print('Class in compression: ', c)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5 and abs(ratio2)<1e-5 and abs(ratio3)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

