# -*- coding: utf-8 -*-
''' Check newGlueNodeToElement function with penalty Newton-Raphson
solution algorithm.

The problem is linear, so there is no need to use a non-linear solver, BUT the 
multi-row, multi-freedom constraints gave an error with this kind of solver. 
This is a regression test for that error.
'''


from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import numpy as np
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F= 1000.0
A= .1 # section area (in**2)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes)

# Define nodes.
n1= modelSpace.newNode(0)
n2= modelSpace.newNode(l)
nA= modelSpace.newNode(l) # node to be glued.

# Define materials.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Define elements.
modelSpace.setElementDimension(1)
modelSpace.setDefaultMaterial(elast)

# Bar.
truss= modelSpace.newElement("Truss", [n1.tag,n2.tag])
truss.sectionArea= A

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode0(n1.tag)

## Glued nodes.
gluedDOFs= xc.ID([0])
loadOnDOFs= [F]

glueA= modelSpace.constraints.newGlueNodeToElement(nA,truss, gluedDOFs)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
loadVector= xc.Vector(loadOnDOFs)
lp0.newNodalLoad(nA.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
# LP 14/08/2026.
# The problem is linear, so there is no need to use a non-linear solver, BUT
# the multi-row, multi-freedom constraints gave an error with this kind of
# solver. This is a regression test for that error.
solProc= predefined_solutions.FullGenNewtonRaphson(feProblem, constraintHandlerType= 'penalty', maxNumIter= 2, printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()
feProblem.logFileName= "/tmp/erase.log" # Don't print warnings.
feProblem.errFileName= "/tmp/erase.err" # Don't print error.
result= analysis.analyze(1)
feProblem.errFileName= "cerr" # Display errors if any.
feProblem.logFileName= "clog" # Display warnings if any.
if(result!= 0):
    # lmsg.error("Can't solve.")
    fname= os.path.basename(__file__)
    lmsg.error(fname+' This test shows a known problem of multi-row multi-freedom constraints that needs to be solved.')
    exit(1)

K= solProc.getA()
B= solProc.getB()

print('K=  ', K)
print('B=', B)

# Compute the solution with numpy
## define the coefficient matrix A
A = np.array(K)
## define the constant vector b
b = np.array(B)
## solve the system of linear equations Ax = b
x = np.linalg.solve(A, b)
print(x)


# Check results.
## Reaction on node A (must be zero).
reactionNodeA= nA.getReaction
ratio1= reactionNodeA.Norm()
## Reaction on node 1 (must be F).
reactionNode1= n1.getReaction
refVector= xc.Vector([F])
ratio2= (reactionNode1+refVector).Norm()
## Reaction on node 2 (must be zero).
reactionNode2= n2.getReaction
ratio3= reactionNode2.Norm()
## Displacement on node A.
nADisp= nA.getDisp[0]
refDisp= F*l/E/A
ratio4= abs(nADisp-refDisp)/-refDisp 

'''
print("ratio1= ", ratio1)
print("reactionNode1= ", reactionNode1)
print("ratio2= ", ratio2)
print("reactionNode2= ", reactionNode2)
print("ratio3= ", ratio3)
print('A glued node displacement: ', nADisp)
print('reference displacement: ', refDisp)
print("ratio4= ", ratio4)
'''

if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp= 'uX', defFScale= 10.0)
