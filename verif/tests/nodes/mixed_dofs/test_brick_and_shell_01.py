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
modelSpace= predefined_spaces.SolidMechanics3D(nodeHandler)

# Materials definition
brickMaterial= typical_materials.defElasticIsotropic3d(preprocessor, "brickMaterial",200000,0.3,0.0)
shellMaterial= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMaterial", E=2.1e6,nu= 0.3, rho= 1.33, h=.1)

# Mesh
## Nodes.
n1= modelSpace.newNodeXYZ(0,0,0)
n2= modelSpace.newNodeXYZ(1,0,0)
n3= modelSpace.newNodeXYZ(1,1,0)
n4= modelSpace.newNodeXYZ(0,1,0)
n5= modelSpace.newNodeXYZ(0,0,1)
n6= modelSpace.newNodeXYZ(1,0,1)
n7= modelSpace.newNodeXYZ(1,1,1)
n8= modelSpace.newNodeXYZ(0,1,1)

modelSpace.setNumberOfDOFs(6)
n15= modelSpace.newNodeXYZ(0,0,1)
n16= modelSpace.newNodeXYZ(1,0,1)
n17= modelSpace.newNodeXYZ(1,1,1)
n18= modelSpace.newNodeXYZ(0,1,1)


## Elements
elementHandler= preprocessor.getElementHandler
### Create brick element.
elementHandler.defaultMaterial= brickMaterial.name
brick= elementHandler.newElement("Brick",xc.ID([n1.tag, n2.tag, n3.tag, n4.tag, n5.tag, n6.tag, n7.tag, n8.tag]))
### Create shell element.
elementHandler.defaultMaterial= shellMaterial.name
elem= elementHandler.newElement("ShellMITC4",xc.ID([n15.tag,n16.tag,n17.tag,n18.tag]))

## Constraints
constrainedNodes= [n1, n2, n3, n4]
n1.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n3.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n3.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
n4.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
### Equal DOFs:
for nA, nB in zip([n5, n6, n7 , n8], [n15, n16, n17, n18]):
    modelSpace.newEqualDOF(nA.tag, nB.tag, dofs= xc.ID([0, 1, 2]))

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
fz= -1000
for n in [n15, n16, n17, n18]:
    lp0.newNodalLoad(n.tag,xc.Vector([0,0,fz,0,0,0]))
    
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
Rz= .0
for n in constrainedNodes:
    Rx+= n.getReaction[0]
    Ry+= n.getReaction[1]
    Rz+= n.getReaction[2]

Fz= 4*fz
ratio1= abs(Rx)/Fz
ratio2= abs(Ry)/Fz
ratio3= (Rz+Fz)/Fz

'''
print('Rx= ', Rx)
print('ratio1= ', ratio1)
print('Ry= ', Ry)
print('ratio2= ', ratio2)
print('Rz= ', Rz)
print('ratio3= ', ratio3)
print('Fz= ', Fz)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # # Output stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# ## Uncomment to display the mesh
# oh.displayFEMesh()
# oh.displayLoads()
