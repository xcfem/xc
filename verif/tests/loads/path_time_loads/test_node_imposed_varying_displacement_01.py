# -*- coding: utf-8 -*-
'''Node under time varying imposed displacement trivial problem.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials import typical_materials
from misc_utils import log_messages as lmsg
from model import predefined_spaces
from solution import predefined_solutions

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes)

# Materials.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",1e9)

# Mesh
L= 0.1
## Node definition.
nod1= modelSpace.newNode(0.0)
nod2= modelSpace.newNode(L)
## Element definition.
modelSpace.setElementDimension(1) # Bar defined in a one-dimensional space.
modelSpace.setDefaultMaterial(elast)
truss= modelSpace.newElement("Truss",[nod1.tag,nod2.tag])
truss.sectionArea= 1e-4

## Constraints
modelSpace.fixNode0(nod1.tag)

## Load pattern definition.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("path_ts","ts")
dispValues= [0.0, 1e-3, 2e-3, 3e-3, 4e-3, 5e-3]
ts.path= xc.Vector(dispValues)
ts.useLast= True
lPatterns.currentTimeSeries= ts.name
lp0= lPatterns.newLoadPattern("default","lp0")
sp= lp0.newSPConstraint(nod2.tag, 0, 1.0)
sp.constant= False

modelSpace.addLoadCaseToDomain(lp0.name)

dt= 1
nSteps= int(len(dispValues)/dt) # Npulse = len(mode_noise)
ts.setTimeIncr(dt)

customSolProc= predefined_solutions.SolutionProcedure(name= 'customSolProc', constraintHandlerType= 'transformation', maxNumIter= 1, convergenceTestTol= 1e-9, printFlag= 0, numSteps= nSteps, numberingMethod= 'rcm', convTestType= None, soeType= 'umfpack_gen_lin_soe', solverType = 'umfpack_gen_lin_solver', integratorType= 'load_control_integrator', solutionAlgorithmType= 'linear_soln_algo', analysisType= 'static_analysis')
customSolProc.feProblem= feProblem
customSolProc.setup()

computedAxialForces= list()
computedDisplacements= list()
steps= list(range(nSteps))
errDisp= 0.0
errForc= 0.0
for i in steps[1:]:
    # Don't warn about no equations.
    if(i==1): feProblem.errFileName= "/tmp/erase.err"
    customSolProc.analysis.analyze(1)
    if(i==1): feProblem.errFileName= "cerr"
    uX= nod2.getDisp[0]
    errDisp+= (uX- dispValues[i])**2
    computedDisplacements.append(uX)
    N= truss.getN()
    errForc+= (N- uX/L*elast.E*truss.sectionArea)**2
    computedAxialForces.append(N)
errDisp= math.sqrt(errDisp)
errForc= math.sqrt(errForc)

'''
print(computedDisplacements, errDisp)
print(computedAxialForces, errForc)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(errDisp)<1e-6) and (abs(errForc)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
