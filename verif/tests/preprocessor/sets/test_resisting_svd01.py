# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function
#    Check of the function that computes the force resultant
#    when cutting a subset of the model by a plane.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Ec= 2e5*9.81/1e-4 # Concrete Young modulus (Pa).
nuC= 0.2 # Concrete Poisson's ratio EHE-08.
hLosa= 0.2 # Thickness.
densLosa= 2500*hLosa # Deck density kg/m2.
# Load
F= 5.5e4 # Load magnitude in N

# active reinforcement
Ep= 190e9 # Elastic modulus expressed in MPa
Ap= 140e-6 # bar area expressed in square meters
fMax= 1860e6 # Maximum unit load of the material expressed in MPa.
fy= 1171e6 # Yield stress of the material expressed in Pa.
tInic= 0.75**2*fMax # Effective prestress (0.75*P0 y 25% prestress losses).

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(1,0,0)
n3= nodes.newNodeXYZ(2,0,0)
n4= nodes.newNodeXYZ(3,0,0)
n5= nodes.newNodeXYZ(0,1,0)
n6= nodes.newNodeXYZ(1,1,0)
n7= nodes.newNodeXYZ(2,1,0)
n8= nodes.newNodeXYZ(3,1,0)
n9= nodes.newNodeXYZ(0,2,0)
n10= nodes.newNodeXYZ(1,2,0)
n11= nodes.newNodeXYZ(2,2,0)
n12= nodes.newNodeXYZ(3,2,0)


# Materials definition

hLosa= typical_materials.defElasticMembranePlateSection(preprocessor, "hLosa",Ec,nuC,densLosa,hLosa)

prestressingSteel= typical_materials.defSteel02(preprocessor, "prestressingSteel",Ep,fy,0.001,tInic)

elements= preprocessor.getElementHandler
# Reinforced concrete deck
elements.defaultMaterial= hLosa.name
elemA= elements.newElement("ShellMITC4",xc.ID([n1.tag, n2.tag, n6.tag, n5.tag]))
elemB= elements.newElement("ShellMITC4",xc.ID([n2.tag, n3.tag, n7.tag, n6.tag]))
elemC= elements.newElement("ShellMITC4",xc.ID([n3.tag, n4.tag, n8.tag, n7.tag]))
elemD= elements.newElement("ShellMITC4",xc.ID([n5.tag, n6.tag, n10.tag, n9.tag]))
elemE= elements.newElement("ShellMITC4",xc.ID([n6.tag, n7.tag, n11.tag, n10.tag]))
elemF= elements.newElement("ShellMITC4",xc.ID([n7.tag, n8.tag, n12.tag, n11.tag]))

# active reinforcement
elements.defaultMaterial= prestressingSteel.name
elements.dimElem= 3 # Dimension of element space
trussA= elements.newElement("Truss",xc.ID([n1.tag, n2.tag]))
trussA.sectionArea= Ap
trussB= elements.newElement("Truss",xc.ID([n2.tag, n3.tag]))
trussB.sectionArea= Ap
trussC= elements.newElement("Truss",xc.ID([n3.tag, n4.tag]))
trussC.sectionArea= Ap
trussD= elements.newElement("Truss",xc.ID([n5.tag, n6.tag]))
trussD.sectionArea= Ap
trussE= elements.newElement("Truss",xc.ID([n6.tag, n7.tag]))
trussE.sectionArea= Ap
trussF= elements.newElement("Truss",xc.ID([n7.tag, n8.tag]))
trussF.sectionArea= Ap
trussG= elements.newElement("Truss",xc.ID([n9.tag, n10.tag]))
trussG.sectionArea= Ap
trussH= elements.newElement("Truss",xc.ID([n10.tag, n11.tag]))
trussH.sectionArea= Ap
trussI= elements.newElement("Truss",xc.ID([n11.tag, n12.tag]))
trussI.sectionArea= Ap

# Constraints

modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n5.tag)
modelSpace.fixNode000_000(n9.tag)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"

lpG= lPatterns.newLoadPattern("default","G")
#lPatterns.currentLoadPattern= "G"
lpG.newNodalLoad(n4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(n8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(n12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

# Combinaciones
combs= loadHandler.getLoadCombinations
comb= combs.newLoadCombination("ELU001","1.00*G")

printFlag= 0

solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")
solAlgo= solutionStrategy.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= solutionStrategy.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-3
ctest.maxNumIter= 10
ctest.printFlag= printFlag
integ= solutionStrategy.newIntegrator("load_control_integrator",xc.Vector([]))
soe= solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","solutionStrategy","")


def solveStaticLinearComb(comb):
  tabComb= comb.tag
  nmbComb= comb.getName
  preprocessor.resetLoadCase()
  comb.addToDomain()
  analOk= analysis.analyze(1)
  comb.removeFromDomain()

o= geom.Pos3d(2,1,0) 
p1= geom.Pos3d(2,2,0)
p2= geom.Pos3d(2,1,1)
plane= geom.Plane3d(o,p1,p2)

def procesResultVerif(comb):
  tabComb= comb.tag
  nmbComb= comb.getName
  setTotal= preprocessor.getSets.getSet("total")
  resultant= setTotal.getResistingSlidingVectorsSystem3d(plane,o,0.01,0)
  global force
  force= resultant.getResultant()
  global moment
  moment= resultant.getMoment()
  org= resultant.getOrg


previousName= ""
tagPrevia= 0
tagSave= 0
for key in combs.getKeys():
  comb= combs[key]
  solveStaticLinearComb(comb)
  procesResultVerif(comb)

ratio1= (abs(force.x-3*F)/3/F)
ratio2= moment.getModulus()

''' 
print("force= ",force/1e3," kN\n")
print("moment= ",moment/1e3," kN\n")
print("org= ",Org," m\n")
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-10) & (ratio2<1e-10) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
