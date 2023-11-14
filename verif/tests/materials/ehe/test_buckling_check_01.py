# -*- coding: utf-8 -*-
''' Test calculation of fictitious eccentricity according to clause 43.5.1 of EHE-08.

Inspired in the test found in: https://calculocivil.com/es/ehe08/pandeo/calc
'''

from __future__ import division
from __future__ import print_function

from materials.ehe import EHE_limit_state_checking
import os
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

ef= EHE_limit_state_checking.get_fictitious_eccentricity(sectionDepth= 0.3, firstOrderEccentricity= 2.14e-2, reinforcementFactor= 1, epsilon_y= 0.0022, radiusOfGyration= 8.66e-2, bucklingLength= 5)
efRef= .05220513834347282
err= abs(ef-efRef)/efRef

'''
print('ef= ', ef*100, 'cm', err)
'''

fname= os.path.basename(__file__)
if(err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
