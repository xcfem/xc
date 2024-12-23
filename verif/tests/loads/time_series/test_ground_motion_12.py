# -*- coding: utf-8 -*-
''' Checks computation of accelerations and velocities from the displacement
    history.

    The equations of motion are as follows:
    x= 10*t**2
    xdot= 20*t
    xdotdot= 20
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
import numpy as np

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

stop= 5.0
DT= .02
num= int(stop/DT)+1
time_values= np.linspace(0.0, stop, num).tolist()
duration= time_values[-1] - time_values[0]
dt= time_values[1] - time_values[0]

#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("constant_ts","ts")
gm= lPatterns.newLoadPattern("uniform_excitation","gm")
mr= gm.motionRecord
hist= mr.history
disp= lPatterns.newTimeSeries("path_ts","disp")
sample= list()
for t in time_values:
    sample.append(10.0*float(t)**2)
disp.path= xc.Vector(sample)
disp.useLast= True
disp.timeIncr= dt
hist.disp= disp
hist.delta= dt

# Compute accelerations and velocities.
displacements= hist.disp.path
velocities= hist.vel.path
accelerations= hist.accel.path

sz= len(accelerations)
errVel= 0.0 # error in velocities.
errAccel= 0.0 # error in accelerations.
# The first and last elements of the velocities and accelerations path are
# incorrect due to the derivative numeric error. Thats why we remove the first
# and last two elements from the lists.
time_values= time_values[2:-2]
accelerations= list(accelerations)[2:-2]
velocities= list(velocities)[2:-2]
for a, v, t in zip(accelerations, velocities, time_values):
    refVel= 20*t
    errVel+= ((refVel-v)/max(refVel,1))**2
    refAccel= 20
    errAccel+= ((refAccel-a)/max(refAccel,1))**2

errVel= math.sqrt(errVel/sz) # root mean square relative error.
errAccel= math.sqrt(errAccel/sz) # root mean square relative error.

'''
print('accelerations: ', accelerations, len(accelerations))
print('errAccel= ', errAccel)
print('velocities: ', velocities, len(velocities))
print('errVel= ', errVel)
print('displacements: ', displacements, len(displacements))
'''


testOK= (errVel<0.001) and (errAccel<0.1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Plot diagram.
# from actions import time_series
# time_series.plot_time_series(timeSeries= accel)
