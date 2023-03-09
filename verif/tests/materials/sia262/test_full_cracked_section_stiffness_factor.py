# -*- coding: utf-8 -*-
'''Compute the stiffness factor for a fully cracked reinforced concrete
   section according to experssion (102) of SIA 262:2013 (clause 4.4.3.2.5).
'''

from materials.sia262 import SIA262_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

ro_tens= 4710e-6/0.6
ro_comp= 2200e-6/0.6
sectionDepth= 0.6
effectiveDepth= 0.54

factor= SIA262_materials.getFullCrackedSectionStiffnesFactor(ro_comp= ro_comp, ro_tens= ro_tens, h= sectionDepth, d= effectiveDepth, phi= 2.5)


ratio1= abs(factor-3.7825724331665165)/3.7825724331665165

'''
print("factor= ", factor)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
