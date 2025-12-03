# -*- coding: utf-8 -*-
'''Classification of steel cross-sections (clause 5.5 of EN 1993-1-1:2005).

Inspired on the Example 2 (page 30) of the presentation: Design of Steel Buildings with worked examples. Rui Simoes. Department of Civil Engineering. University of Coimbra. Brussels 16-17 October 2014.
 
https://eurocodes.jrc.ec.europa.eu/sites/default/files/2022-06/05_Eurocodes_Steel_Workshop_SIMOES.pdf
'''

from __future__ import print_function
from __future__ import division


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

from materials.ec3 import EC3_materials

steelMaterial= EC3_materials.S355JR

steelShape= EC3_materials.IPEShape(steel=steelMaterial,name='IPE_400')
c1_2= steelShape.getClassInternalPartInBending()
c1_3= steelShape.getClassOutstandPartInCompression()
c= steelShape.getClassInBending()
ratio1= abs(c1_2-1)
ratio2= abs(c1_3-1)
ratio3= abs(c-1)

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

