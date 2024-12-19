# -*- coding: utf-8 -*-
'''  Checks computation of accelerations and velocities from displacements
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
disp= lPatterns.newTimeSeries("path_ts","disp")
sample= list()
for i in range(0,20):
    sample.append(10.0*float(i)**2)
disp.path= xc.Vector(sample)
disp.useLast= True
hist.disp= disp
hist.delta= 1.0

# Compute accelerations and velocities.
displacements= hist.disp.path
velocities= hist.vel.path
accelerations= hist.accel.path

sz= len(accelerations)

errVel= 0.0 # error in velocities.
errAccel= 0.0 # error in accelerations.
for i in range(3,sz-4): # The values at the beggining and at the end of the
    t= float(i)         # sequence are wrong due to unavoidable numerical
    refVel= 20*t        # errors.
    errVel+= ((refVel-velocities[i])/max(refVel,1))**2
    refAccel= 20
    errAccel+= ((refAccel-accelerations[i])/max(refAccel,1))**2

errAccel= math.sqrt(errAccel/sz) # root mean square relative error.
errVel= math.sqrt(errVel/sz) # root mean square relative error.

'''
print('accelerations: ', accelerations, len(accelerations))
print('errAccel= ', errAccel)
print('velocities: ', velocities, len(velocities))
print('errVel= ', errVel)
print('displacements: ', displacements, len(displacements))
'''

testOK= (errAccel<0.001) and (errVel<0.001)

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
