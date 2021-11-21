# -*- coding: utf-8 -*-
''' Test constant time series. '''
from __future__ import print_function

import math
import xc_base
import geom
import xc

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("path_ts","ts")
ts.path= xc.Vector([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 78.79296057779986, 118.96357325970033, 207.7719942356257, 311.49999999999994, 346.28665705937533, 356.8907197791001, 551.5507240445967, 616.0000000000001, 461.76790037029866, 805.182133701498, 725.7794717226295, 385.00000000000006, 564.495144673155, 483.1092802208992, 256.5377224279428, 308.0000000000003, 236.37888173339815, 118.96357325970004, 69.2573314118754, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
ts.setTimeIncr(0.1388888888888889)
duration= ts.getDuration()
ratio1= abs(duration-14.027777777777779)/14.027777777777779
peakFactor= ts.getPeakFactor()
peakFactorRef= ts.path[30]
ratio2= abs(peakFactor-peakFactorRef)/peakFactorRef

''' 
print('duration: ', duration)
print('ratio1= ', ratio1)
print('peak factor: ', peakFactor)
print('reference peak factor: ', peakFactorRef)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio2)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
