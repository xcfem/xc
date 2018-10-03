# -*- coding: utf-8 -*-

import xc_base
import geom
import xc

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

motionDuration= 0.0
motionPath= None 
motionPathSize= 0
motionFactor= 0.0
motionPeakFactor= 0.0
motionLastSendCommitTag= 0.0
motionPathTimeIncrement= 0.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor


#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("constant_ts","ts")
gm= lPatterns.newLoadPattern("uniform_excitation","gm")
mr= gm.motionRecord
hist= mr.history
accel= lPatterns.newTimeSeries("path_ts","accel")
accel.path= xc.Vector([1,2,3,4])
hist.accel= accel
hist.delta= 0.01

motionDuration= mr.getDuration() 
motionPath= mr.history.accel.path
motionPathSize= mr.history.getNumberOfDataPoints()
motionFactor= mr.history.accel.getFactor(0.5)
motionPeakFactor= mr.history.accel.getPeakFactor
motionLastSendCommitTag= mr.history.accel.lastSendCommitTag
motionPathTimeIncrement= mr.history.accel.getTimeIncr(0.5)

ratio1= (motionDuration-4)/4
ratio2= (motionFactor-1.5)/1.5
ratio3= (motionPeakFactor-4)/4
ratio4= (motionLastSendCommitTag+1)
ratio5= (motionPathTimeIncrement-1)
ratio6= (motionPathSize-4)/4

''' 
print "duration= ",motionDuration
print "ratio1= ",ratio1
print "path= ",motionPath
print "factor= ",motionFactor
print "ratio2= ",ratio2
print "peak factor= ",motionPeakFactor
print "ratio3= ",ratio3
print "lastSendCommitTag= ",motionLastSendCommitTag
print "ratio4= ",ratio4
print "path time increment= ",motionPathTimeIncrement
print "ratio5= ",ratio4
print "path size= ",motionPathSize
print "ratio6= ",ratio6
  '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15) & (abs(ratio6)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
