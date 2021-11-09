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


scaleFactor= 1
begin= 0
end= 2
period= 1
shift= 0
#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("triangular_ts","ts")
ts.factor= scaleFactor
ts.tStart= begin
ts.tFinish= end
ts.period= period
ts.shift= shift

duration= ts.getDuration()
durationRef= end-begin
t= 3.0*period/4.0
factor= ts.getFactor(t)
factorRef= -scaleFactor
peakFactor= ts.getPeakFactor()
peakFactorRef= scaleFactor
pathTimeIncrement= ts.getTimeIncr(0.5)


ratio1= abs(duration-durationRef)/durationRef
ratio2= abs(factor-factorRef)/factorRef
ratio3= (peakFactor-peakFactorRef)/peakFactorRef
ratio4= (pathTimeIncrement-2.0)

'''
print("duration= ",duration)
print("durationRef= ",durationRef)
print("ratio1= ",ratio1)
print("factor= ",factor)
print("refFactor= ",factorRef)
print("ratio2= ",ratio2)
print("peak factor= ",peakFactor)
print("ratio3= ",ratio3)
print("path time increment= ",pathTimeIncrement)
print("ratio4= ",ratio4)
  '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
