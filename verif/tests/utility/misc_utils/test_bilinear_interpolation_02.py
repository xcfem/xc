# -*- coding: utf-8 -*-
''' Test linear interpolation class.'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
from misc_utils import math_utils

xT= [6,8,10,12] # nominal wall thickness (inches)
yT= [0.0,16.0,24.0,32.0,40.0,48.0,56.0,64.0,72.0,1e6] # spacing inches
zT= [[5.62, 7.62, 9.62, 11.62],
     [3.7, 4.9, 5.98, 7.04],
     [3.13, 4.1, 4.91, 5.7],
     [2.85, 3.7, 4.37, 5.02],
     [2.68, 3.46, 4.05, 4.62],
     [2.57, 3.3, 3.83, 4.35],
     [2.49, 3.19, 3.67, 4.16],
     [2.42, 3.1, 3.56, 4.01],
     [2.38, 3.03, 3.47, 3.90],
     [2.00, 2.50, 2.75, 3.00]]
fEquivalentThickness= math_utils.StaticGridBilinearInterpolator(yT, xT, zT)

def get_equivalent_thickness(xT, yT):
    ''' Return the equivalent wall thickness according to
            table 5-2 of TM 5-809-3.'''
    retval= fEquivalentThickness(np.array([yT]), np.array([xT]))
    retval= float(retval[0])
    return retval

err= 0.0
for j, xt in enumerate(xT):
    for i, yt in enumerate(yT):
        et= get_equivalent_thickness(xt, yt)
        print(et, zT[i][j])
        err+=(et- zT[i][j])**2
        
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
