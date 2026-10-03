# -*- coding: utf-8 -*-
'''Trivial dynamic analysis of an SDOF system using an HHT integrator. Regression test.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import numpy as np
import xc
from model import predefined_spaces
from materials import typical_materials
from actions import time_series
from solution import hht
from actions.quake import ground_motion_utils as gmu
from misc_utils import log_messages as lmsg

silent= True # If true don't show debug messages nor graphics.

# Create FE problem
feProblem= xc.FEProblem()
prep=feProblem.getPreprocessor
nodeHandler= prep.getNodeHandler
elements= prep.getElementHandler
# One DOF one-dimensional space.
modelSpace= predefined_spaces.SolidMechanics1D(nodeHandler) 

# Define mesh.
## Geometry.
n1= modelSpace.newNodeX(0.0)
n2= modelSpace.newNodeX(0.0)
n2.mass= xc.Matrix([[1.0]]) # Mass = 1.0 kg at node 2.

## Define elements.
### Define materials.
k_x= 100.0 # N/m
kX= typical_materials.defElasticMaterial(prep, "kX", k_x)
### Define zero length element.
modelSpace.setDefaultMaterial(kX)
modelSpace.setElementDimension(1)
zl= modelSpace.newElement("ZeroLength", [n1.tag,n2.tag])


## Constraints.
modelSpace.fixNode('0', n1.tag) # Fix node 1.

# Define dynamic loading (Sinusoidal force)
## Create a time series for a sine function: Force = sin(w * t)
load_frequency_w= 5.0  # rad/s
period= 2.0 * np.pi/ load_frequency_w
ts= time_series.def_trig_time_series(preprocessor= prep, name= 'ts', tStart= 0.0, tEnd= 10.0, period= period)
modelSpace.setCurrentTimeSeries(ts.name)
sineLP= modelSpace.newLoadPattern(name= 'sineLP', setCurrent= True)
sineLP.newNodalLoad(n2.tag, xc.Vector([10.0]))
modelSpace.addLoadCaseToDomain(sineLP.name) # Append load pattern to domain.

dt = 0.02  # Time step
total_time = 5.0  # Total simulation duration
steps = int(total_time / dt)
'''
sineLPTs= sineLP.timeSeries
for step in range(steps):
    currentTime= step*dt
    print(currentTime, sineLPTs.getFactor(currentTime))
'''

# Solution procedure.
alpha_hht = 0.9  # Introduces moderate numerical damping
solProc= hht.HHT(prb= feProblem, name= 'hht', timeStep= dt, maxNumIter= 10, convergenceTestTol= 1e-6, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', alpha= alpha_hht, solutionAlgorithmType= 'newton_raphson_soln_algo', constraintHandlerType= 'plain')
# from solution import transient
# solProc= transient.PlainNewmarkNewtonRaphson(prb= feProblem, numSteps= 1, timeStep= dt, maxNumIter= 1, convTestType= 'norm_unbalance_conv_test', printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()

domain= modelSpace.getDomain()
ti= list()
xi= list()
for step in range(steps):
    # Advance the simulation by one time step
    analysis.analyze(1, dt)
    # Retrieve real-time results
    ti.append(domain.currentTime)
    xi.append(n2.getDisp[0])

# Check results.
uXExtrema= gmu.MinMaxTracker()
for t, ux in zip(ti, xi):
    uXExtrema.update(ux, t)

# Check the analysis goes as expected.
uXMinRef= -0.1792411538484657
ratio1= abs(uXExtrema.min-uXMinRef)/uXMinRef
tUXMinRef= 4.619999999999946
ratio2= abs(uXExtrema.t_min-tUXMinRef)/tUXMinRef
uXMaxRef= 0.17282952237433039
ratio3= abs(uXExtrema.max-uXMaxRef)/uXMaxRef
tUXMaxRef= 0.4200000000000001
ratio4= abs(uXExtrema.t_max-tUXMaxRef)/tUXMaxRef

if(not silent):
    # print(uXExtrema.getDict())
    print(ratio1)
    print(ratio2)
    print(ratio3)
    print(ratio4)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio2)<1e-12 and abs(ratio3)<1e-12 and abs(ratio4)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Display results
if(not silent):
    import matplotlib.pyplot as plt
    plt.ylabel("Displacement")
    plt.xlabel("Time")
    plt.plot(ti, xi)
    plt.show()
