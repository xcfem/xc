# -*- coding: utf-8 -*-
''' Trivial test of EN 13674-1 flat bottom rails.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials.sections.structural_shapes import bs_en_13674_1_rail_sections

testSection= bs_en_13674_1_rail_sections.RAIL['RAIL_UIC54']

h= testSection['h']
ratio1= abs(h-.159)/.159
a= testSection['A']
aRef= testSection['P']/7850.0
ratio2= abs(a-aRef)/aRef

'''
print(h, ratio1)
print(a*1e4, 'cm2')
print(aRef*1e4, 'cm2')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
