# -*- coding: utf-8 -*-
''' Test path time series. '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import json
import os
import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Get the path to the data file
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
path_time_values_file_path= pth+'/data/test_path_time_values.json'
path_time_values= json.load( open(path_time_values_file_path, 'r'))


# Get load modulation from JSON file.
timeValues= path_time_values['timeValues']
loadValues= path_time_values['loadValues']

loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("path_time_ts","ts")
ts.path= xc.Vector(loadValues)
ts.time= xc.Vector(timeValues)

duration= ts.getDuration()
refDuration= timeValues[-1]-timeValues[0]
ratio1= abs(duration-refDuration)/refDuration
peakFactor= ts.getPeakFactor()
peakFactorRef= ts.path[30]
ratio2= abs(peakFactor-peakFactorRef)/peakFactorRef

''' 
print('duration: ', duration)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
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
    
# # Plot diagram.
# from actions import time_series
# time_series.plot_time_series(timeSeries= ts)
