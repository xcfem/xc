# -*- coding: utf-8 -*-
''' Trivial frictional soil verification test.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from geotechnics import frictional_soil as fs
import math
from scipy.constants import g

phi= math.radians(32)
gammaMPhi= 1.25
soil= fs.FrictionalSoil(phi= phi, rho= 1.8e3/g, gammaMPhi= gammaMPhi)

designPhi= soil.getDesignPhi()
designPhiRef= math.atan(math.tan(phi)/gammaMPhi)
ratio= abs(designPhi-designPhiRef)/designPhiRef

'''
print('design value of internal friction: ', math.degrees(designPhi))
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio<1e-10):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
