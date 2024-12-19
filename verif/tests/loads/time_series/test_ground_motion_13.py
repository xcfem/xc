# -*- coding: utf-8 -*-
''' Checks computation of velocities and displacements from accelearion history
    using the Simpson integrator.

    The equations of motion are as follows:
    x= 10*t**2
    xdot= 20*t
    xdotdot= 20
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor


#Load modulation.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("constant_ts","ts")
gm= lPatterns.newLoadPattern("uniform_excitation","gm")
mr= gm.motionRecord
hist= mr.history
hist.setIntegrator('simpson')
accel= lPatterns.newTimeSeries("path_ts","accel")
accel.path= xc.Vector([20,20,20,20,20,20,20,20,20])
accel.useLast= True
hist.accel= accel
hist.delta= 1.0

accelerations= hist.accel.path
velocities= hist.vel.path
displacements= hist.disp.path

sz= len(accelerations)
errVel= 0.0 # error in velocities.
errDisp= 0.0 # error in displacements.
for i in range(0,sz):
    t= float(i)
    refVel= 20*t
    errVel+= ((refVel-velocities[i])/max(refVel,1))**2
    refDisp= 10*t**2
    errDisp+= ((refDisp-displacements[i])/max(refDisp,1))**2
errVel= math.sqrt(errVel/sz) # root mean square relative error.
errDisp= math.sqrt(errDisp/sz) # root mean square relative error.

'''
print('errVel= ', errVel)
print('errDisp= ', errDisp)
'''

testOK= (errVel<0.001) and (errDisp<0.001)

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
