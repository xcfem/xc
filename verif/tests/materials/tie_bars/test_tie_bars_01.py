# -*- coding: utf-8 -*-
''' Home made test. Data from: https://www.s3i.co.uk/carbon-steel-tie-bar.php'''
from __future__ import print_function

from materials.tie_bars import tie_bars

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

tieBars= [tie_bars.M12, tie_bars.M16, tie_bars.M20, tie_bars.M24, tie_bars.M30, tie_bars.M36, tie_bars.M42, tie_bars.M48, tie_bars.M56, tie_bars.M64, tie_bars.M76, tie_bars.M90, tie_bars.M100]
yieldLoads= [30e3, 82e3, 127e3, 184e3, 292e3, 425e3, 583e3, 766e3, 1056e3, 1392e3,1999e3, 2879e3, 3605e3]

ultimateLoads= [51e3, 104e3, 162e3, 233e3, 370e3, 539e3, 740e3, 972e3, 1340e3, 1766e3, 2537e3, 3654e3, 4575e3]

ratio= 0.0
for b, yLRef, uLRef in zip(tieBars, yieldLoads, ultimateLoads):
    yL= b.getNominalYieldStrength()
    ratio+= abs(yL-yLRef)/yLRef
    uL= b.getNominalTensileStrength()
    ratio+= abs(uL-uLRef)/uLRef

# print('ratio= ', ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

