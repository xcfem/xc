# -*- coding: utf-8 -*-
''' Test for snow loads according to Swiss standard (SIA).'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.snow import ASCE7_snow

p_g= 30.0

pf= ASCE7_snow.designSnowLoadFlatRoof(terrainCategory= 'B', roofExposure= 'partially', thermalCondition= 'A', riskCategory= 2, p_g= p_g)
pfRef= 21.0
ratio1= abs(pf-pfRef)/pfRef

'''
print("pf=", pf)
print("pfRef=", pfRef)
print("ratio1= ", ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
