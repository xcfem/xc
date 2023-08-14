# -*- coding: utf-8 -*-
''' Test path time series. '''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns

#### Create path time series TS1.
timeValues= [0, 10, 100]
loadValues= [0, 1, 1]
ts1= lPatterns.newTimeSeries("path_time_ts","ts")
ts1.path= xc.Vector(loadValues)
ts1.time= xc.Vector(timeValues)
#### Create path time series TS2.
loadValues= [0, 1, 5]
ts2= lPatterns.newTimeSeries("path_time_ts","ts2")
ts2.path= xc.Vector(loadValues)
ts2.time= xc.Vector(timeValues)

duration1= ts1.getDuration()
refDuration1= timeValues[-1]-timeValues[0]
ratio11= abs(duration1-refDuration1)/refDuration1
peakFactor1= ts1.getPeakFactor()
peakFactorRef1= ts1.path[2]
ratio12= abs(peakFactor1-peakFactorRef1)/peakFactorRef1
factor10= ts1.getFactor(10)
ratio13= abs(factor10-1)
factor11= ts1.getFactor(36)
ratio14= abs(factor11-1)

duration2= ts2.getDuration()
refDuration2= timeValues[-1]-timeValues[0]
ratio21= abs(duration2-refDuration2)/refDuration2
peakFactor2= ts2.getPeakFactor()
peakFactorRef2= ts2.path[2]
ratio22= abs(peakFactor2-peakFactorRef2)/peakFactorRef2
factor20= ts1.getFactor(10)
ratio23= abs(factor20-1)
factor21= ts2.getFactor(36)
ratio24= abs(factor21-2.1555555555555554)

''' 
print('duration1: ', duration1)
print('ratio1= ', ratio1)
print('peak factor: ', peakFactor)
print('reference peak factor: ', peakFactorRef)
print('ratio2= ', ratio2)
print('ts1(t= 10)=', factor10)
print('ratio13= ', ratio13)
print('ts1(t= 36)=', factor11)
print('duration2: ', duration2)
print('ratio21= ', ratio21)
print('peak factor: ', peakFactor2)
print('reference peak factor: ', peakFactorRef2)
print('ratio22= ', ratio22)
print('ts2(t= 10)=', factor20)
print('ratio23= ', ratio23)
print('ts2(t= 36)=', factor21)
print('ratio24= ', ratio24)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio11)<1e-15) and (abs(ratio12)<1e-15) and (abs(ratio13)<1e-15) and (abs(ratio14)<1e-15) and (abs(ratio21)<1e-15) and (abs(ratio22)<1e-15)and (abs(ratio23)<1e-15) and (abs(ratio24)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Plot diagram.
# from actions import time_series
# time_series.plot_time_series(timeSeries= ts1)
# time_series.plot_time_series(timeSeries= ts2)
