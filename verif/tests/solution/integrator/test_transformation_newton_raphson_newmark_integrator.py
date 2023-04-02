# -*- coding: utf-8 -*-
''' Trivial test to check plain linear Newmark solver. Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000.0 # Spring constant
l= 1e-7 # Distance between nodes
F= 1 # Force magnitude

import math
import xc
from solution import predefined_solutions
from materials import typical_materials

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
nodeHandler.dimSpace= 1 # One coordinate for each node.
nodeHandler.numDOFs= 1 # One degree of freedom for each node.

n1= nodeHandler.newNodeX(1)
n2= nodeHandler.newNodeX(1.0+l)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",K)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 1 #Element dimension.
zl= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1


# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("linear_ts","ts") # Load increases linearly with time.
lPatterns.currentTimeSeries= ts.name
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"
nl= lp0.newNodalLoad(n2.tag,xc.Vector([F]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Static solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
nodeHandler.calculateNodalReactions(True,1e-7)

# Dynamic analysis.
## Define recorders
dFree= []
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([n2.tag]))
recDFree.callbackRecord= "dFree.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
## 
duration= 10
dT= 0.1
numberOfSteps= int(duration/dT)
solProc= predefined_solutions.TransformationNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= dT)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

error= 0.0
ti= list()
xi= list()
for values in dFree:
    time= values[0]
    ti.append(time)
    disp= values[1][0]
    xi.append(disp)
    fTeor= F*time
    dispTeor= fTeor/K
    error+= (disp-dispTeor)**2

error= math.sqrt(error)

# print('error= ', error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
 
# # Display results.
# import matplotlib.pyplot as plt
# plt.plot(ti, xi, '-')
# plt.grid()
# plt.show()
