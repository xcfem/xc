# -*- coding: utf-8 -*-
# home made test
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
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod1= nodes.newNodeXYZ(0,0,0)
nod2= nodes.newNodeXYZ(1,0,0)
nod3= nodes.newNodeXYZ(2,0,0)
nod4= nodes.newNodeXYZ(3,0,0)
nod5= nodes.newNodeXYZ(0,1,0)
nod6= nodes.newNodeXYZ(1,1,0)
nod7= nodes.newNodeXYZ(2,1,0)
nod8= nodes.newNodeXYZ(3,1,0)
nod9= nodes.newNodeXYZ(0,2,0)
nod10= nodes.newNodeXYZ(1,2,0)
nod11= nodes.newNodeXYZ(2,2,0)
nod12= nodes.newNodeXYZ(3,2,0)


# Materials definition

hLosa= typical_materials.defElasticMembranePlateSection(preprocessor, "hLosa",Ec,nuC,densLosa,hLosa)

typical_materials.defSteel02(preprocessor, "prestressingSteel",Ep,fy,0.001,tInic)

elements= preprocessor.getElementHandler
# Reinforced concrete deck
elements.defaultMaterial= "hLosa"
elements.defaultTag= 1
elem= elements.newElement("ShellMITC4",xc.ID([1,2,6,5]))

elem= elements.newElement("ShellMITC4",xc.ID([2,3,7,6]))
elem= elements.newElement("ShellMITC4",xc.ID([3,4,8,7]))
elem= elements.newElement("ShellMITC4",xc.ID([5,6,10,9]))
elem= elements.newElement("ShellMITC4",xc.ID([6,7,11,10]))
elem= elements.newElement("ShellMITC4",xc.ID([7,8,12,11]))

# active reinforcement
elements.defaultMaterial= "prestressingSteel"
elements.dimElem= 3 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([1,2]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([2,3]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([3,4]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([5,6]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([6,7]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([7,8]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([9,10]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([10,11]))
truss.sectionArea= Ap
truss= elements.newElement("Truss",xc.ID([11,12]))
truss.sectionArea= Ap

# Constraints

modelSpace.fixNode000_000(1)
modelSpace.fixNode000_000(5)
modelSpace.fixNode000_000(9)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"

lpG= lPatterns.newLoadPattern("default","G")
#lPatterns.currentLoadPattern= "G"
lpG.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

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
analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= analysisAggregation.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-3
ctest.maxNumIter= 10
ctest.printFlag= printFlag
integ= analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
soe= analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","analysisAggregation","")


def resuelveCombEstatLin(comb):
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
  resuelveCombEstatLin(comb)
  procesResultVerif(comb)

ratio1= (abs(force.x-3*F)/3/F)
ratio2= moment.getModulus()

''' 
print "force= ",force/1e3," kN\n"
print "moment= ",moment/1e3," kN\n"
print "org= ",Org," m\n"
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-10) & (ratio2<1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
