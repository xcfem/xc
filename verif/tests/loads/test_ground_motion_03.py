# -*- coding: utf-8 -*-
'''  Checks ground_motion_record function
    by integrating the accelerations calculated as follows:
    x= 9*t**3+10*t**2
    xdot= 27*t**2+20*t
    xdotdot= 54*t+20  '''

import xc_base
import geom
import xc

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

DT= 0.01

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor


#Load modulation.
cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("constant_ts","ts")
gm= casos.newLoadPattern("uniform_excitation","gm")
mr= gm.motionRecord
hist= mr.history
accel= casos.newTimeSeries("path_ts","accel")
accel.path= xc.Vector([20,74,128,182,236,290,344,398])
hist.accel= accel
hist.delta= DT

motionHistoryDuration= mr.getDuration()
motionHistoryPathSize= mr.history.getNumberOfDataPoints()
motionHistoryAccel= mr.getAccel(0.5)
motionHistoryPeakAccel= mr.getPeakAccel()
motionHistoryDelta= mr.history.delta
motionHistoryVel= mr.getVel(0.5)
motionHistoryPeakVel= mr.getPeakVel()
motionHistoryDisp= mr.getDisp(0.5)
motionHistoryPeakDisp= mr.getPeakDisp()

ratio1= (motionHistoryDuration-8)/8
ratio2= (motionHistoryAccel-47)/47
ratio3= (motionHistoryPeakAccel-398)/398
ratio4= (motionHistoryDelta-DT)/DT
ratio5= (motionHistoryVel-16.75)/16.75
ratio6= (motionHistoryPathSize-8)/8
ratio7= (motionHistoryPeakVel-1458)/1458
ratio8= (motionHistoryDisp-3.63)/3.63

''' 
print "duration= ",motionHistoryDuration
print "ratio1= ",ratio1
print "accel= ",motionHistoryAccel
print "ratio2= ",ratio2
print "peak accel= ",motionHistoryPeakAccel
print "ratio3= ",ratio3
print "delta= ",motionHistoryDelta
print "ratio4= ",ratio4
print "vel= ",motionHistoryVel
print "ratio5= ",ratio5
print "path size= ",motionHistoryPathSize
print "ratio6= ",ratio6
print "peak vel= ",motionHistoryPeakVel
print "ratio7= ",ratio7
print "disp= ",motionHistoryDisp
print "ratio8= ",ratio8
  '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<motionHistoryDelta) & (abs(ratio6)<1e-15) & (abs(ratio7)<5*motionHistoryDelta)& (abs(ratio8)<5*motionHistoryDelta):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
