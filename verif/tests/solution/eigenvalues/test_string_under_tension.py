# -*- coding: utf-8 -*-
''' Test basado en el denominado VM53 of the Ansys verification manual
   Reference: Thomson, Ref 9: Vibration Theory and Applications, pg. 264, clause 8.2.
   '''
from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc

from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 13
E= 30e6 # Young modulus (psi)
l= 100.0 # String length in inches
sigmaPret= E*0.00543228 # Prestressing force (pounds)
area= 0.00306796 # Section area in square inches.
Mass= 0.00073*area # Mass per unit length.
MassNod= Mass*(l/NumDiv) # Mass per unit length.
fPret= sigmaPret*area # Prestressing force (pounds)


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)


# Materials definition
cable= typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,Mass)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= cable.name
seedElemHandler.dimElem= 2 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
truss= seedElemHandler.newElement("CorotTruss",xc.ID([0,0]))
truss.sectionArea= area
# seed element definition ends

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(l,0.0,0.0))
lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)
    
# Constraints
predefined_spaces.ConstraintsForLineExtremeNodes(l1,modelSpace.fixNode000)
predefined_spaces.ConstraintsForLineInteriorNodes(l1,modelSpace.fixNodeFF0)


Nstep= 10  #  apply load in 10 steps
DInc= 1./Nstep 	#  first load increment
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("plain_handler")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")
solAlgo= solutionStrategy.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= solutionStrategy.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-8
ctest.maxNumIter= 100
integ= solutionStrategy.newIntegrator("load_control_integrator",xc.Vector([]))
integ.dLambda1= DInc
soe= solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","solutionStrategy","")
result= analysis.analyze(Nstep)

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
tension= ele1.getN()
sigma= ele1.getMaterial().getStress()


# Solution procedure

#solu.clearAll()
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")
solAlgo= solutionStrategy.newSolutionAlgorithm("frequency_soln_algo")
integ= solutionStrategy.newIntegrator("eigen_integrator",xc.Vector([]))
soe= solutionStrategy.newSystemOfEqn("sym_band_eigen_soe")
solver= soe.newSolver("sym_band_eigen_solver")
analysis= solu.newAnalysis("eigen_analysis","solutionStrategy","")


analOk= analysis.analyze(3)
eig1= analysis.getEigenvalue(1)
eig2= analysis.getEigenvalue(2)
eig3= analysis.getEigenvalue(3)
f1= math.sqrt(eig1)/(2*math.pi)
f2= math.sqrt(eig2)/(2*math.pi)
f3= math.sqrt(eig3)/(2*math.pi)



ratio1= abs((sigma-162974)/162974)
ratio2= abs((tension-500)/500)

ratio3= abs((f1-74.708)/74.708)
ratio4= abs((f2-149.42)/149.42)
ratio5= abs((f3-224.12)/224.12)

''' 
print("stress= ",sigma)
print("ratio1= ",(ratio1))
print("tension= ",tension)
print("ratio2= ",(ratio2))

print("eig1= ",eig1)
print("eig2= ",eig2)
print("eig3= ",eig3)
print("f1= ",math.sqrt(eig1)/(2*math.pi))
print("ratio3= ",(ratio3))
print("f2= ",math.sqrt(eig2)/(2*math.pi))
print("ratio4= ",(ratio4))
print("f3= ",math.sqrt(eig3)/(2*math.pi))
print("ratio5= ",(ratio5))
   '''
 
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-2) & (abs(ratio3)<1e-2) & (abs(ratio5)<5e-2) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
  
