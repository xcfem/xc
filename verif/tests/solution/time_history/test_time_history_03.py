# -*- coding: utf-8 -*-
'''Trivial check for PlainNewmarkNewtonRampshon solution procedure.'''

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
from solution import predefined_solutions
from actions.quake import ground_motion_utils as gmu
from misc_utils import log_messages as lmsg

silent= True

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

# Perform an eigenvalue analysis-
analysis= predefined_solutions.frequency_analysis(feProblem)
analOk= analysis.analyze(1) # Compute 1 eigenvalue.
eigenvalues= analysis.getEigenvalues()
eigenvaluesTable= list([['Eigenvalues at start of transient'],
                        ["lambda","omega","period","frequency"]])
for lambdA in eigenvalues:
    omega= math.sqrt(lambdA)
    period= 2*math.pi/omega
    freq= 1/period
    eigenvaluesTable.append(["{:.3e}".format(lambdA), "{:.4f}".format(omega), "{:.4f}".format(period), "{:.4f}".format(freq)])
if(not silent):
    import tabulate
    print(tabulate.tabulate(eigenvaluesTable))
    
# Dynamic analysis.
## Define RECORDERS
domain= modelSpace.getDomain()
cDisp= list()
recDisp= domain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID([n2.tag]))
recDisp.callbackRecord= "cDisp.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
cAccel= list()
recAccel= domain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID([n2.tag]))
recAccel.callbackRecord= "cAccel.append([self.getDomain.getTimeTracker.getCurrentTime,self.getAccel])"


## Define dynamic loads.
## Read the excitation data.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)
timeValues= el_centro_raw[:,0]
# Be extra careful when extracting acceleration values. If readint the data
# using the wrong procedure the time data can be taken as accelerations too.
accelValues= el_centro_raw[:,1]

timeStep= .02 # Time step for the excitation sample.
scale= 1.0
## Create horizontal acceleration load pattern.
xGM, xGMSize= gmu.get_uniform_excitation_from_accel_values(modelSpace, name= "xGM", dof= 0, accelValues= list(accelValues), dt= timeStep, cod_ts= 'xAccel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(xGM.name)

## Set the Rayleigh damping factors for nodes & elements.
alphaM= 0.02 # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm=  0.0 # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)


## Perform the analysis.
numberOfSteps= 2*xGMSize
domain.setTime(0.0) # initialize time.
solProc= predefined_solutions.PlainNewmarkNewtonRaphson(prb= feProblem, numSteps= numberOfSteps, timeStep= timeStep, maxNumIter= 1, convTestType= 'energy_incr_conv_test', printFlag= 0)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

uXExtrema= gmu.MinMaxTracker()
for t, ux in cDisp:
    uXExtrema.update(ux[0], t)
aXExtrema= gmu.MinMaxTracker()
for t, ax in cAccel:
    aXExtrema.update(ax[0], t)

# Check results.
refUxDict= {'min': -0.09897163226374574, 't_min': 26.399999999999526, 'max': 0.09953642581381053, 't_max': 26.65999999999952}
refAxDict= {'min': -15.748154993570607, 't_min': 27.6599999999995, 'max': 15.517578072567902, 't_max': 26.399999999999526}

err= 0.0
uXDict= uXExtrema.getDict()
for key in uXDict:
    value= uXDict[key]
    refValue= refUxDict[key]
    err+= (value-refValue)**2

aXDict= aXExtrema.getDict()
for key in aXDict:
    value= aXDict[key]
    refValue= refAxDict[key]
    err+= (value-refValue)**2
err= math.sqrt(err)
testOK= err<1e-12

if(not silent):
    print(uXDict)
    print(aXDict)
    print(err)

fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Display results
if(not silent):
    import matplotlib.pyplot as plt
    ti, Dxi= zip(*cDisp)
    plt.ylabel("Displacement")
    plt.xlabel("Time")
    plt.plot(ti, Dxi)
    plt.show()
    ti, Axi= zip(*cAccel)
    plt.ylabel("Acceleration")
    plt.xlabel("Time")
    plt.plot(ti, Axi)
    plt.show()

