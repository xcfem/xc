# -*- coding: utf-8 -*-
''' Check newGlueNodeToElement function with penalty Newton-Raphson solution
algorithm.

The problem is linear, so there is no need to use a non-linear solver, BUT the 
multi-row, multi-freedom constraints gave an error with this kind of solver. 
This is a regression test for that error.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from postprocess.quick_inquiry import nodal_reactions
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Nodes.
n1= modelSpace.newNode(0,0,0)
n2= modelSpace.newNode(1,0,0)
n3= modelSpace.newNode(1,1,0)
n4= modelSpace.newNode(0,1,0)

nA= modelSpace.newNode(0.5,0.5,0.0) # node to be glued.

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
for n in [n1,n2,n3,n4]:
  modelSpace.fixNode000_FFF(n.tag)

## Glued node.
gluedDOFs= [0,1,2,3,4,5]
for i in range(0,6):
  if i not in gluedDOFs:
    modelSpace.constraints.newSPConstraint(nA.tag,i,0.0)

glue= modelSpace.glueNodeToElement(nA.tag, elem.tag, xc.ID(gluedDOFs))

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
F= 1000
loadVector= xc.Vector([0, 0, -F, 0, 0, 0])
lp0.newNodalLoad(nA.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
# LP 14/08/2026.
# The problem is linear, so there is no need to use a non-linear solver, BUT
# the multi-row, multi-freedom constraints gave an error with this kind of
# solver. This is a regression test for that error.
analysis= predefined_solutions.penalty_newton_raphson(feProblem, maxNumIter= 2) # => KO
result= analysis.analyze(1)
if(result!= 0):
    lmsg.error("Can't solve.")
    exit(1)

nodes.calculateNodalReactions(False,1e-7)

reactionNodeA= nA.getReaction
ratio1= reactionNodeA.Norm()
reactions= list()
for n in [n1, n2, n3, n4]:
    reactions.append(n.getReaction)

refReactions= 4*[(0.0, 0.0, F/4)]
error= 0.0
for R, refR in zip(reactions, refReactions):
    for i in [0, 1]:
        error+= (R[i]-refR[i])**2
error= math.sqrt(error)

print("ratio1= ", ratio1)
print("error= ", error)

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(error)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayLocalAxes()
# oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 100.0)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 100.0)
# oh.displayLoads()
# oh.displayReactions()
