# -*- coding: utf-8 -*-
'''Trivial dynamic analysis of an SDOF system.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg


# *** PROBLEM
feProblem= xc.FEProblem()
prep=feProblem.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes) #defines dimension of
                  #the space: nodes by two coordinates (x,y) and 
                  #two DOF for each node (Ux,Uy)

## *** MESH ***                  
### *** GEOMETRY ***
n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(0.0,0.0)

### Single point constraints -- Boundary Conditions
constraints= prep.getBoundaryCondHandler
modelSpace.fixNode00(n1.tag)
modelSpace.fixNodeF0(n2.tag)

### nodal masses:
mass= 10/(2*math.pi)**2
n2.mass= xc.Matrix([[mass,0],[0,0]])  # node mass matrix.

### Define materials.
k_x= 1000.0
kX= typical_materials.defElasticMaterial(prep, "kX",k_x)
### Define ELEMENTS 
elems= modelSpace.getElementHandler()
elems.dimElem= 2 # space dimension.
elems.defaultMaterial= kX.name
zl= elems.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))
zl.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))

# Define RECORDERS
cDisp= list()
recDisp= prep.getDomain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID([n2.tag]))
recDisp.callbackRecord= "cDisp.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
cAccel= list()
recAccel= prep.getDomain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID([n2.tag]))
recAccel.callbackRecord= "cAccel.append([self.getDomain.getTimeTracker.getCurrentTime,self.getAccel])"


## Initial displacement.
x0= .01
v0= 0.0
n2.setTrialDisp(xc.Vector([x0, 0]))
prep.getDomain.commit() # Commit the initial displacement.
phi= math.pi/2.0
amplitudeRef= x0/math.sin(phi)

## Solution
w= math.sqrt(k_x/mass)
T= 2*math.pi/w
f= 1.0/T
duration= 2*T # two cycles.
timeStep= 1/(25*f)
numberOfSteps= int(duration/timeStep)+1
aMaxRef= amplitudeRef*w**2
### Dynamic analysis.
prep.getDomain.setTime(0.0)
solProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= numberOfSteps, timeStep= timeStep)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()
    
# Extract results.
ti= list()
Dxi= list() # displacement.
dxMax= 0.0
dxMin= 0.0
Axi= list() # acceleration.
xdotdotMax= 0.0
xdotdotMin= 0.0
for rx,ra in zip(cDisp,cAccel):
    ti.append(rx[0])
    x= rx[1][0] # x displacement.
    Dxi.append(x)
    dxMax= max(x,dxMax)
    dxMin= min(x,dxMin)
    xdotdot= ra[1][0] # x acceleration.
    xdotdotMax= max(xdotdot,xdotdotMax)
    xdotdotMin= min(xdotdot,xdotdotMin)
    Axi.append(xdotdot)

amplitude= (dxMax-dxMin)/2.0
ratio1= abs(amplitude-amplitudeRef)/amplitudeRef
aMax= abs(xdotdotMax-xdotdotMin)/2.0
ratio2= abs(aMax-aMaxRef)/aMaxRef

'''
print('pulsation: ', w, ' rad/s')
print('natural frequency: ', f, ' Hz')
print('time step: ', timeStep, ' s')
print('max disp: x_max= ', dxMax, ' m')
print('min disp: x_min= ', dxMin, ' m')
print('amplitude: ', amplitude, ' s')
print('amplitudeRef: ', amplitudeRef, ' s')
print('ratio1: ', ratio1)
print('min acceleration: a_min= ', xdotdotMin, ' m/s2')
print('max acceleration: a_max= ', xdotdotMax, ' m/s2')
print('reference acceleration AmaxRef: ', aMaxRef, ' m/s2')
print('ratio2: ', ratio2)
'''

# # Display results
# import matplotlib.pyplot as plt
# plt.plot(ti, Dxi)
# plt.show()

fname= os.path.basename(__file__)
if((abs(ratio1)<.01) and (ratio2<.01)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
