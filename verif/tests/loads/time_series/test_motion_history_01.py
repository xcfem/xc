# -*- coding: utf-8 -*-
'''Trivial check for PenaltyNewmarkKrylovNewtonMUMPS solution procedure.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import xc
import numpy as np
from scipy.constants import g
from model import predefined_spaces
from materials import typical_materials
from actions.quake import ground_motion_utils as gmu
from misc_utils import log_messages as lmsg

silent= False

# *** PROBLEM
feProblem= xc.FEProblem()
prep=feProblem.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes) # defines dimension of
                                                      # the space: nodes by one
                                                      # coordinate (x) and 
                                                      # one DOF for each node
                                                      # (Ux)

## *** MESH ***                  
### *** GEOMETRY ***
n1= nodes.newNodeX(0.0)
n2= nodes.newNodeX(0.0)

### Single point constraints -- Boundary Conditions
constraints= prep.getBoundaryCondHandler
modelSpace.fixNode0(n1.tag)

### nodal masses:
mass= 250/(2*math.pi)**2
n2.mass= xc.Matrix([[mass]])  # node mass matrix.

### Define materials.
k_x= 1000.0
kX= typical_materials.defElasticMaterial(prep, "kX",k_x)
### Define ELEMENTS 
elems= modelSpace.getElementHandler()
elems.dimElem= 1 # space dimension.
elems.defaultMaterial= kX.name
zl= elems.newElement("ZeroLength",xc.ID([n1.tag, n2.tag]))
zl.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))

# Read the excitation data.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)
timeValues= el_centro_raw[:,0]
accelValues= el_centro_raw[:,1]

timeStep= .02 # Time step for the excitation sample.
scale= 1.0
## Create horizontal acceleration load pattern.
xGM, xGMSize= gmu.get_uniform_excitation_from_accel_values(modelSpace, name= "xGM", dof= 0, accelValues= list(accelValues), dt= timeStep, cod_ts= 'xAccel', factor= g*scale, vel0= 0.0)
mr= xGM.motionRecord
hist= mr.history

accelHist= hist.accel
timeIncr= accelHist.timeIncr
ratio1= (timeStep-timeIncr)/timeIncr
prependZero= accelHist.getPrependZero()
testOK= (abs(ratio1)<1e-12) and (not prependZero)

# Check PathSeries::getFactor method.
err= 0.0
cFactor= accelHist.cFactor
for i in range(0, xGMSize):
    t= i*timeStep
    accel= accelHist.getFactor(t)
    refAccel= cFactor*accelValues[i]
    err+= (refAccel-accel)**2
err= math.sqrt(err)
testOK&= (err<1e-11)

# Check MotionHistory::getAccel method.
err= 0.0
for i in range(0, xGMSize):
    t= i*timeStep
    accel= hist.getAccel(t)
    refAccel= cFactor*accelValues[i]
    err+= (refAccel-accel)**2
err= math.sqrt(err)
testOK&= (err<1e-11)

'''
print('time increment: ', timeIncr)
print('ratio1= ', ratio1)
print('err= ', err)
print('test OK: ', testOK)
'''

fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
