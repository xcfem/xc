# -*- coding: utf-8 -*-
''' Checks computation of accelerations and displacements from velocities 
    history.

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
hist.setIntegrator('trapezoidal')
vel= lPatterns.newTimeSeries("path_ts","vel")
vel.path= xc.Vector([0, 20, 40, 60, 80, 100, 120, 140, 160, 180])
vel.useLast= True
hist.vel= vel
hist.delta= 1.0

velocities= hist.vel.path
accelerations= hist.accel.path
displacements= hist.disp.path

sz= len(velocities)
errAccel= 0.0 # error in velocities.
errDisp= 0.0 # error in displacements.
for i in range(0,sz):
    t= float(i)
    refAccel= 20.0
    errAccel+= ((refAccel-accelerations[i])/max(refAccel,1))**2
    refDisp= 10*t**2
    errDisp+= ((refDisp-displacements[i])/max(refDisp,1))**2
errAccel= math.sqrt(errAccel/sz) # root mean square relative error.
errDisp= math.sqrt(errDisp/sz) # root mean square relative error.

'''
print(accelerations, len(accelerations))
print('errAccel= ', errAccel)
print(velocities, len(velocities))
print(displacements, len(displacements))
print('errDisp= ', errDisp)
'''

testOK= (errAccel<0.1) and (errDisp<0.2)

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
