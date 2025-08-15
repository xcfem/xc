# -*- coding: utf-8 -*-
''' Test a model mixing a brick and a shell element. The shell element
    expects 6 DOF by node and the brick element needs only 3.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Materials definition
quadMaterial= typical_materials.defElasticIsotropic3d(preprocessor, "quadMaterial",200000,0.3,0.0)
beamMaterial= typical_materials.defElasticSection2d(preprocessor, "scc", A= 0.2*0.3, E= 30e9, I= 1/12*0.2*0.3**3)

# Mesh
## Nodes.
n1= modelSpace.newNodeXY(0,0)
n2= modelSpace.newNodeXY(1,0)
n3= modelSpace.newNodeXY(1,1)
n4= modelSpace.newNodeXY(0,1)

modelSpace.setNumberOfDOFs(3)
n13= modelSpace.newNodeXY(1,1)
n14= modelSpace.newNodeXY(0,1)

## Elements
### Create quad element.
modelSpace.setDefaultMaterial(quadMaterial)
quad= modelSpace.newElement("FourNodeQuad",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag]))
### Create beam element.
modelSpace.setDefaultMaterial(beamMaterial)
transfCooHandler= preprocessor.getTransfCooHandler
lin= transfCooHandler.newLinearCrdTransf2d("lin")
modelSpace.setDefaultCoordTransf(lin)
elem= modelSpace.newElement("ElasticBeam2d",xc.ID([n14.tag,n13.tag]))

## Constraints
constrainedNodes= [n1, n2]
n1.fix(xc.ID([0,1]),xc.Vector([0,0]))
n2.fix(xc.ID([0,1]),xc.Vector([0,0]))

### Equal DOFs:
for nA, nB in zip([n3, n4], [n13, n14]):
    modelSpace.newEqualDOF(nA.tag, nB.tag, dofs= xc.ID([0, 1]))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
fy= -1000
for n in [n13, n14]:
    lp0.newNodalLoad(n.tag,xc.Vector([0,fy,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution.

## DISCUSSION ABOUT THE SOLUTION PROCEDURE:
## The linear solver gives poor results
## The Krylov Newton gives excellent results but is a bit slow (to check why).
## The penalty Newton-Raphson gives good results but only converges
## if the energy increment convergence test is used and the tolerances
## are increased a bit.
## Finally the static linear solver using a transformation constraint handler
## gives excellent results and is really fast.

### Static linear solver.
# result= modelSpace.analyze(calculateNodalReactions= True)
### Penalty Krylov Newton solver
# solProc= predefined_solutions.PenaltyKrylovNewton(feProblem, printFlag= 0)
# solProc= predefined_solutions.PenaltyNewtonRaphson(feProblem, convergenceTestTol= 1e-7, convTestType= 'energy_incr_conv_test', printFlag= 0)
# solProc.solveComb(lp0.name, calculateNodalReactions= True, reactionCheckTolerance= 1e-7)
### Static linear solver with with transformation constraint handler.
solProc= predefined_solutions.SimpleTransformationStaticLinear(feProblem, printFlag= 0)
solProc.solveComb(lp0.name, calculateNodalReactions= True)

# Compute reactions.
Rx= .0
Ry= .0
for n in constrainedNodes:
    Rx+= n.getReaction[0]
    Ry+= n.getReaction[1]

Fy= 2*fy
ratio1= abs(Rx)/Fy
ratio2= (Ry+Fy)/Fy

'''
print('Rx= ', Rx)
print('ratio1= ', ratio1)
print('Ry= ', Ry)
print('ratio2= ', ratio2)
print('Fy= ', Fy)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) and (abs(ratio2)<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Output stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
