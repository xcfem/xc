# -*- coding: utf-8 -*-
''' Test linear time series. '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("linear_ts","ts")
err= 0.0
duration= 100
for time in range(0,duration+1):
    factor= ts.getFactor(time)
    err+= (factor-time)**2
err= math.sqrt(err)

''' 
print("err ",err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
