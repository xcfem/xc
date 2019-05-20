# -*- coding: utf-8 -*-
# home made test

E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math
from postprocess.quick_inquiry import nodal_reactions
from solution import predefined_solutions

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler


p1= geom.Pos3d(0,0,0)
p2= geom.Pos3d(1,0,0)
p3= geom.Pos3d(1,1,0)
p4= geom.Pos3d(0,1,0)

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,p1.x,p1.y,p1.z)
nodes.newNodeIDXYZ(2,p2.x,p2.y,p2.z)
nodes.newNodeIDXYZ(3,p3.x,p3.y,p3.z)
nodes.newNodeIDXYZ(4,p4.x,p4.y,p4.z)

p10= geom.Pos3d(0.5,0.5,0.0)
n10= nodes.newNodeIDXYZ(10,p10.x,p10.y,p10.z)

# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC4",xc.ID([1,2,3,4]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000_FFF(1)
modelSpace.fixNode000_FFF(2)
modelSpace.fixNode000_FFF(3)
modelSpace.fixNode000_FFF(4)
#modelSpace.fixNode000_000( 1)
#modelSpace.fixNode000_000( 2)
#modelSpace.fixNode000_000( 3)
#modelSpace.fixNode000_000( 4)

#Glued node.
gluedDOFs= [0,1,2,3,4,5]
loadOnDOFs= [0,0,0,0,0,0]
for i in range(0,6):
  if i not in gluedDOFs:
    modelSpace.constraints.newSPConstraint(10,i,0.0)
  else:
    loadOnDOFs[i]= -1000.0

glue= modelSpace.constraints.newGlueNodeToElement(n10,elem,xc.ID(gluedDOFs))

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(10,xc.Vector(loadOnDOFs))
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# Solution
# Solution
solver= predefined_solutions.SolutionProcedure()
analisis= solver.simpleLagrangeStaticLinear(feProblem)
result= analisis.analyze(1)
nodes.calculateNodalReactions(False,1e-7)

reactionNode10= nodes.getNode(10).getReaction
ratio1= reactionNode10.Norm()
svdReactionNodes= nodal_reactions.getReactionFromNodes(nodes,"UVWRxRyRz",[1,2,3,4])
actionNode10= xc.Vector(loadOnDOFs)
actionNode10Norm= actionNode10.Norm()
svdAction= nodal_reactions.getSlidingVectorsSystemfromSlidingVector("UVWRxRyRz",n10.get3dCoo,actionNode10)
svdResid= svdReactionNodes+svdAction
ratio2= svdResid.getResultant().getModulo()/actionNode10Norm
ratio3= svdResid.getMoment().getModulo()/actionNode10Norm

# print "svdAction= ", svdAction
# print "svdReactionNodes= ", svdReactionNodes
# print "svdResid= ", svdResid
# print "ratio1= ", ratio1
# print "ratio2= ", ratio2
# print "ratio3= ", ratio3
# print "RN2= ", RN2
# print "RN3= ", RN3
# print "RN4= ", RN4

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9) & (result==0):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
