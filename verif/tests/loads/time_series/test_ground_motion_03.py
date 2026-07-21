# -*- coding: utf-8 -*-
'''  Checks ground_motion_record function
    by integrating the accelerations calculated as follows:
    x= 9*t**3+10*t**2
    xdot= 27*t**2+20*t
    xdotdot= 54*t+20  '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

DT= 0.01

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

x_i= list()
xdot_i= list()
xdotdot_i= list()
for t in range(0,8):
    x= 9*t**3+10*t**2
    x_i.append(x)
    xdot= 27*t**2+20*t
    xdot_i.append(xdot)
    xdotdot= 54*t+20
    xdotdot_i.append(xdotdot)

#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("constant_ts","ts")
gm= lPatterns.newLoadPattern("uniform_excitation","gm")
mr= gm.motionRecord
hist= mr.history
accel= lPatterns.newTimeSeries("path_ts","accel")
accel.path= xc.Vector(xdotdot_i)
hist.accel= accel
hist.delta= DT

motionHistoryDuration= mr.getDuration()
motionHistoryPathSize= mr.history.getNumberOfDataPoints()
motionHistoryAccel= mr.getAccel(0.5)
motionHistoryAccelRef= 54*0.5+20
motionHistoryPeakAccel= mr.getPeakAccel()
motionHistoryPeakAccelRef= max(xdotdot_i)
motionHistoryDelta= mr.history.delta
motionHistoryVel= mr.getVel(0.5)
motionHistoryVelRef= 27*0.5**2+20*0.5
motionHistoryPeakVel= mr.getPeakVel()
motionHistoryPeakVelRef= max(xdot_i)
motionHistoryDisp= mr.getDisp(0.5)
motionHistoryDispRef= 9*0.5**3+10*0.5**2
motionHistoryPeakDisp= mr.getPeakDisp()
motionHistoryPeakDispRef= 9*8**3+10*8**2

ratio1= (motionHistoryDuration-8)/8
ratio2= (motionHistoryAccel-motionHistoryAccelRef)/motionHistoryAccelRef
ratio3= (motionHistoryPeakAccel-398)/398
ratio4= (motionHistoryDelta-DT)/DT
ratio5= (motionHistoryVel-motionHistoryVelRef)/motionHistoryVelRef
ratio6= (motionHistoryPathSize-8)/8
ratio7= (motionHistoryPeakVel-motionHistoryPeakVelRef)/motionHistoryPeakVelRef
ratio8= (motionHistoryDisp-motionHistoryDispRef)/motionHistoryDispRef
ratio9= (motionHistoryPeakDisp-motionHistoryPeakDispRef)/motionHistoryPeakDispRef

''' 
print("duration= ",motionHistoryDuration)
print("ratio1= ",ratio1)
print("accel= ",motionHistoryAccel)
print("refAccel= ",motionHistoryAccelRef)
print("ratio2= ",ratio2)
print("peak accel= ",motionHistoryPeakAccel)
print("ratio3= ",ratio3)
print("delta= ",motionHistoryDelta)
print("ratio4= ",ratio4)
print("vel= ",motionHistoryVel)
print("ratio5= ",ratio5)
print("path size= ",motionHistoryPathSize)
print("ratio6= ",ratio6)
print("peak vel= ",motionHistoryPeakVel)
print("ratio7= ",ratio7)
print("disp= ",motionHistoryDisp)
print("ratio8= ",ratio8)
print("peak disp= ",motionHistoryPeakDisp)
print("reference peak disp= ",motionHistoryPeakDispRef)
print("ratio9= ",ratio9)
  '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
tol= 5*motionHistoryDelta
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<motionHistoryDelta) & (abs(ratio6)<1e-15) & (abs(ratio7)<tol) & (abs(ratio8)<tol) & (abs(ratio9)<tol):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Plot diagram.
# from actions import time_series
# time_series.plot_time_series(timeSeries= accel)
