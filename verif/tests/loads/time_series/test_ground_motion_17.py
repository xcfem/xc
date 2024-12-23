# -*- coding: utf-8 -*-
'''  Test based on the example shown here: https://github.com/OpenSees/OpenSees/pull/1557#issuecomment-2558158234
'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
import numpy as np
from scipy.integrate import cumulative_trapezoid
from model import predefined_spaces
from solution import predefined_solutions

time_values= np.linspace(0.0, 5.0, 251)
duration= time_values[-1] - time_values[0]
dt= time_values[1] - time_values[0]
accel_values= np.sin(2.0 * np.pi * time_values)
vel_values= cumulative_trapezoid(accel_values, time_values, initial=0)
disp_values= cumulative_trapezoid(vel_values, time_values, initial=0)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Create time series for acceleration, velocity, and displacement
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
dummyTs= lPatterns.newTimeSeries("constant_ts","dummyTs") # Dummy time series.
zeroLP= lPatterns.newLoadPattern('default', 'zeroLP')
## Acceleration load pattern.
accelLP= lPatterns.newLoadPattern("uniform_excitation","accel")
accelLP.factor= -1.0
accelLP.dof= 0 # excitation along the global X axis
accelSeries= accelLP.motionRecord
hist= accelSeries.history
hist.accel= lPatterns.newTimeSeries("path_time_ts","accel_ts")
hist.accel.path= xc.Vector(accel_values.tolist())
hist.accel.time= xc.Vector(time_values.tolist())
## Velocities load pattern.
velLP= lPatterns.newLoadPattern("uniform_excitation","vel")
velLP.factor= -1.0
velLP.dof= 0 # excitation along the global X axis
velSeries= velLP.motionRecord
hist= velSeries.history
hist.vel= lPatterns.newTimeSeries("path_time_ts","vel_ts")
hist.vel.path= xc.Vector(vel_values.tolist())
hist.vel.time= xc.Vector(time_values.tolist())
hist.delta= dt

## Displacements load pattern.
dispLP= lPatterns.newLoadPattern("uniform_excitation","disp")
dispLP.factor= -1.00
dispLP.dof= 0 # excitation along the global X axis
dispSeries= dispLP.motionRecord
hist= dispSeries.history
hist.disp= lPatterns.newTimeSeries("path_time_ts","disp_ts")
hist.disp.path= xc.Vector(disp_values.tolist())
hist.disp.time= xc.Vector(time_values.tolist())
hist.delta= dt

# Create FE model.
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) # defines dimension
                                                           # of the space: nodes
                                                           # by two coordinates
                                                           # (x,y) and three
                                                           #  DOF for each node
                                                           # (Ux,Uy,Theta)
node= modelSpace.newNode(0,0)
node.mass= xc.Matrix([[1,0,0],[0,1,0],[0,0,1]])  # node mass matrix.
# Constraints.
modelSpace.fixNode('F00', node.tag)

## Define RECORDERS
#### Record node displacements.
dFree= list() # Create a list to store (time, displacement) pairs.
#### Create the recorder that will be called each time that
#### convergence is achieved:
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
#### Tell the recorders which nodes will be tracked.
recDFree.setNodes(xc.ID([node.tag]))
#### Define the Python code that will be executed each time that
#### convergence is achieved:
recDFree.callbackRecord= "dFree.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp[0]])"

# Define the solution procedure.
Nsteps = int(duration/dt)
solProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= Nsteps, timeStep= dt)

load_patterns= [accelLP, velLP, dispLP]
disp_results= list()
for lp in load_patterns:
    modelSpace.addLoadCaseToDomain(lp.name)
    # Run transient analysis
    if(solProc.solve()!=0):
        lmsg.error('Transient analysis failed.')
        quit()
    # Get displacements from the recorder.
    disp_results.append(dFree.copy())
    dFree.clear()
    modelSpace.removeLoadCaseFromDomain(lp.name)
    modelSpace.revertToStart()
    solProc.clear()

# Check results.
errAccel= 0.0
for dispA, disp in zip(disp_results[0], disp_values.tolist()):
    errAccel= (dispA[1]-disp)**2
errAccel= math.sqrt(errAccel)
errVel= 0.0
for dispV, disp in zip(disp_results[1], disp_values.tolist()):
    errVel= (dispV[1]-disp)**2
errVel= math.sqrt(errVel)
errDisp= 0.0
for dispD, disp in zip(disp_results[2], disp_values.tolist()):
    errDisp= (dispD[1]-disp)**2
errDisp= math.sqrt(errDisp)

testOK= (errAccel<1e-4) and (errVel<1e-2) and (errDisp<1e-2)

'''
print(errAccel)
print(errVel)
print(errDisp)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # Display results.
# import matplotlib.pyplot as plt
# plt.plot(time_values, disp_values, label="Analytical Disp", linewidth=2, zorder=0)
# plt.xlabel("Time")
# plt.ylabel("Displacement")
# for results, lp in zip(disp_results, load_patterns):
#     time_results, disp_results= zip(*results)
#     plt.plot(time_results, disp_results, label="Case "+str(lp.name), linestyle='--', lw=2)

# plt.title("Comparison of Displacement for All Cases")
# plt.legend()
# plt.grid()
# plt.show()
