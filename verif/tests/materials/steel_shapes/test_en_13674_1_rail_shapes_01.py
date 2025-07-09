# -*- coding: utf-8 -*-
''' Trivial test of EN 13674-1 flat bottom rails.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials.sections.structural_shapes import bs_en_13674_1_rail_sections

testSection= bs_en_13674_1_rail_sections.RAIL['RAIL_54E1']

h= testSection['h']
ratio= abs(h-.159)/.159

# print(h, ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
