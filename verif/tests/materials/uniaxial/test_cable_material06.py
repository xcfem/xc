# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions.basic_loads import nodal_loads

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 100
E= 30e6 # Young modulus (psi)
lng= 10 # Cable length in inches
sigmaPret= 1500 # Prestressing force (pounds)
area= 2
fPret= sigmaPret*area # Prestressing force (pounds)
F= 100/NumDiv # Carga vertical

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)



# Define materials
cable= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,0.0)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= cable.name
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 # Number for the next element will be 1.
truss= seedElemHandler.newElement("CorotTruss",xc.ID([1,2]))
truss.sectionArea= area
# seed element definition ends

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(lng,lng,0.0))
lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
predefined_spaces.ConstraintsForLineExtremeNodes(l,modelSpace.fixNode000_000)
predefined_spaces.ConstraintsForLineInteriorNodes(l,modelSpace.fixNodeFFF_000)


# Load case definition.
lPattern= '0'
lp0= modelSpace.newLoadPattern(name= lPattern)
modelSpace.setCurrentLoadPattern(lPattern)
nodal_loads.load_on_nodes_in_line(l1,lp0,xc.Vector([0,-F,0,0,0,0]))
modelSpace.addLoadCaseToDomain(lPattern)


Nstep= 10  #  apply load in 10 steps
DInc= 1./Nstep 	#  first load increment

solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
cHandler= sm.newConstraintHandler("plain_handler")
analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-6
ctest.maxNumIter= 100
integ= analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
integ.dLambda1= DInc
soe= analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","analysisAggregation","")
result= analysis.analyze(Nstep)

tagN1= l.firstNode.tag
tagN2= l.lastNode.tag
index= int(NumDiv/2)+1
tagN3= l.getNodeI(index).tag

nodes.calculateNodalReactions(True,1e-7)
R1X= nodes.getNode(tagN2).getReaction[0]
R1Y= nodes.getNode(tagN2).getReaction[1] 
R2X= nodes.getNode(tagN1).getReaction[0]
R2Y= nodes.getNode(tagN1).getReaction[1] 
deltaX= nodes.getNode(tagN3).getDisp[0]
deltaY= nodes.getNode(tagN3).getDisp[1]  


alpha= -math.atan2(deltaY,lng/2)
Ftot= (NumDiv-1)*F
ratio1= ((R1X+R2X)/fPret)
ratio2= ((R1Y+R2Y-Ftot)/Ftot)
    
''' 
print("tagN1= ",tagN1)
print("tagN2= ",tagN2)
print("tagN3= ",tagN3)
print("F= ",(F))
print("alpha= ",rad2deg((alpha)))
print("R1X= ",R1X)
print("R1Y= ",R1Y)
print("R2X= ",R2X)
print("R2Y= ",R2Y)
print("deltaX= ",deltaX)
print("deltaY= ",deltaY)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
   '''
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11) & (abs(ratio2)<1e-9) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
