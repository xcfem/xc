# -*- coding: utf-8 -*-
''' Test of PySimple1 uniaxial material object.'''

__author__= "Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es "


import os
import xc
import math
from model import predefined_spaces
from materials import soil_structure_interaction as ssi
from solution import transient
from actions.quake import ground_motion_utils as gmu
from misc_utils import log_messages as lmsg

silent= False

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
## Problem type
modelSpace= predefined_spaces.SolidMechanics1D(preprocessor.getNodeHandler)

# Define mesh.
n1= modelSpace.newNode(0.0) # Free node.
n1.mass= xc.Matrix([[10.0]]) # Pile mass.
n2= modelSpace.newNode(0.0) # Fixed node.
# Define constraints.
modelSpace.fixNode('0', n2.tag)
# Define soil-structura interaction material.
pyS1= ssi.def_pysimple1_material(preprocessor,
                                 matName= 'pyS1',
                                 soilType= 1, # Soft clay.
                                 pult= 50.0,
                                 Y50= 0.01,
                                 c= 0.0,
                                 Cd= 0.0)
# Define "spring" element.
modelSpace.setDefaultMaterial(pyS1)
modelSpace.setElementDimension(1)
soilSpring= modelSpace.newElement("ZeroLength", [n1.tag,n2.tag])

# Dynamic analysis setup.
initial_vel= 0.5 # high initial velocity to force soil plastification.
n1.setTrialVel(xc.Vector([initial_vel]))
preprocessor.getDomain.commit()

# Solution procedure.
timeStep= .01
solProc= transient.PlainNewmarkNewtonRaphson(prb= feProblem, numSteps= 0, timeStep= timeStep, maxNumIter= 10, convTestType= 'norm_unbalance_conv_test', printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()

displacements= list()
forces= list()
uXExtrema= gmu.MinMaxTracker()
fXExtrema= gmu.MinMaxTracker()
numSteps= 400

for step in range(numSteps):
    analysis.analyze(1, timeStep)
    time= step*timeStep
    uX=n1.getDisp[0]
    displacements.append(uX)
    uXExtrema.update(uX, time)
    modelSpace.calculateNodalReactions(includeInertia= True, reactionCheckTolerance= 1e-5)
    F= n2.getReaction[0]
    forces.append(F)
    fXExtrema.update(F, time)
    
uXmin= uXExtrema.min
err= (uXmin-0.003464799897535181)**2
tUXmin= uXExtrema.t_min
err+= (tUXmin-0.39)**2
uXmax= uXExtrema.max
err+= (uXmax-0.04101685685722802)**2
tUXmax= uXExtrema.t_max
err+= (tUXmax-0.13)**2
fXmin= fXExtrema.min
err+= (fXmin--42.13324835728948)**2
tFXmin= fXExtrema.t_min
err+= (tFXmin-0.13)**2
fXmax= fXExtrema.max
err+= (fXmax-23.66486751307802)**2
tFXmax= fXExtrema.t_max
err+= (tFXmax-0.39)**2
err= math.sqrt(err)

'''
print('Minimum displacement: ', uXmin, ' at time: ', tUXmin)
print('Maximum displacement: ', uXmax, ' at time: ', tUXmax)
print('Minimum soil reaction: ', fXmin, ' at time: ', tFXmin)
print('Maximum soil reaction: ', fXmax, ' at time: ', tFXmax)
print(err)
'''

import os
fname= os.path.basename(__file__)
if(err<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Plot the cyclic loading response.
if(not silent):
    import matplotlib.pyplot as plt
    plt.figure(figsize=(8, 5))
    plt.plot(displacements, forces, color='b', linewidth=1.5)
    plt.title('Soil hysteretic damping (PySimple1)')
    plt.xlabel('Pile displacement (m)')
    plt.ylabel('Soil reaction (kN)')
    plt.grid(True)
    plt.show()
