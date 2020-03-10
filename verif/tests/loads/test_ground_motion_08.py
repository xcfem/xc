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

groundMotionDuration= 0.0
groundMotionAccel= 0.0
groundMotionPeakAccel= 0.0
motionHistoryDelta= 0.0
groundMotionVel= 0.0
groundMotionPeakVel= 0.0
groundMotionDisp= 0.0
groundMotionPeakDisp= 0.0

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
accel.path= xc.Vector([20,74,128,182,236,290,344,398])
hist.accel= accel
hist.delta= 0.01

motionHistoryDelta= hist.delta
groundMotionDuration= mr.getDuration()
groundMotionAccel= mr.getAccel(0.5)
groundMotionPeakAccel= mr.getPeakAccel()
groundMotionVel= mr.getVel(0.5)
groundMotionPeakVel= mr.getPeakVel()
groundMotionDisp= mr.getDisp(0.5)
groundMotionPeakDisp= mr.getPeakDisp()

ratio1= (groundMotionDuration-8)/8
ratio2= (groundMotionAccel-47)/47
ratio3= (groundMotionPeakAccel-398)/398
ratio4= (motionHistoryDelta-0.01)/0.01
ratio5= (groundMotionVel-16.75)/16.75
ratio7= (groundMotionPeakVel-1458)/1458
ratio8= (groundMotionDisp-3.63)/3.63

''' 
print "duration= ",groundMotionDuration
print "ratio1= ",ratio1
print "accel= ",groundMotionAccel
print "ratio2= ",ratio2
print "peak accel= ",groundMotionPeakAccel
print "ratio3= ",ratio3
print "delta= ",motionHistoryDelta
print "ratio4= ",ratio4
print "vel= ",groundMotionVel
print "ratio5= ",ratio5
print "peak vel= ",groundMotionPeakVel
print "ratio7= ",ratio7
print "disp= ",groundMotionDisp
print "ratio8= ",ratio8
  '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<motionHistoryDelta) & (abs(ratio7)<motionHistoryDelta) & (abs(ratio8)<2*motionHistoryDelta) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
