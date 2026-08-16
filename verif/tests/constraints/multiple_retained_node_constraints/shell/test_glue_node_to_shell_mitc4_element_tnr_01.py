# -*- coding: utf-8 -*-
''' Check newGlueNodeToElement function with transformation Newton-Raphson
solution algorithm.

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

import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from postprocess.quick_inquiry import nodal_reactions
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem type
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
loadVector= xc.Vector([0, 0, -1000, 0, 0, 0])
lp0.newNodalLoad(nA.tag, loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
# LP 14/08/2026.
# The problem is linear, so there is no need to use a non-linear solver, BUT
# the multi-row, multi-freedom constraints gave an error with this kind of
# solver. This is a regression test for that error.
analysis= predefined_solutions.transformation_newton_raphson_band_gen(feProblem, maxNumIter= 10, printFlag=0) # => OK 14/08/2026.
result= analysis.analyze(1)
if(result!= 0):
    lmsg.error("Can't solve.")
    exit(1)

nodes.calculateNodalReactions(False,1e-7)

reactionNodeA= nA.getReaction
ratio1= reactionNodeA.Norm()
svdReactionNodes= nodal_reactions.getReactionFromNodes(nodes,"UVWRxRyRz",elem.getNodes.getExternalNodes)
actionNodeA= loadVector
actionNodeANorm= actionNodeA.Norm()
svdAction= nodal_reactions.getSlidingVectorsSystemfromSlidingVector("UVWRxRyRz",nA.get3dCoo,actionNodeA)
svdResid= svdReactionNodes+svdAction
ratio2= svdResid.getResultant().getModulus()/actionNodeANorm
ratio3= svdResid.getMoment().getModulus()/actionNodeANorm

# print("svdAction= ", svdAction)
# print("svdReactionNodes= ", svdReactionNodes)
# print("svdResid= ", svdResid)
# print("ratio1= ", ratio1)
# print("ratio2= ", ratio2)
# print("ratio3= ", ratio3)
# print("RN2= ", RN2)
# print("RN3= ", RN3)
# print("RN4= ", RN4)

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 100.0)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 100.0)
# oh.displayReactions()
# oh.displayLocalAxes()
# oh.displayLoads()
# #oh.displayLocalAxes()
