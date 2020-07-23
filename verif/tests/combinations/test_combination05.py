# -*- coding: utf-8 -*-
from __future__ import print_function
'''Using the database as combination results storage to accelerate computation. Home made test.'''

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
F= 5.5e4 # Load magnitude en N

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
from solution import database_helper as dbHelper

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(1,0,0)
nod= nodes.newNodeXYZ(2,0,0)
nod= nodes.newNodeXYZ(3,0,0)
nod= nodes.newNodeXYZ(0,1,0)
nod= nodes.newNodeXYZ(1,1,0)
nod= nodes.newNodeXYZ(2,1,0)
nod= nodes.newNodeXYZ(3,1,0)
nod= nodes.newNodeXYZ(0,2,0)
nod= nodes.newNodeXYZ(1,2,0)
nod= nodes.newNodeXYZ(2,2,0)
nod= nodes.newNodeXYZ(3,2,0)


# Materials definition

hLosa= typical_materials.defElasticMembranePlateSection(preprocessor, "hLosa",Ec,nuC,densLosa,hLosa)

prestressingSteel= typical_materials.defSteel02(preprocessor, "prestressingSteel",Ep,fy,0.001,tInic)

elements= preprocessor.getElementHandler
# Reinforced concrete deck
elements.defaultMaterial= hLosa.name
elements.defaultTag= 1
elem= elements.newElement("ShellMITC4",xc.ID([1,2,6,5]))

elem= elements.newElement("ShellMITC4",xc.ID([2,3,7,6]))
elem= elements.newElement("ShellMITC4",xc.ID([3,4,8,7]))
elem= elements.newElement("ShellMITC4",xc.ID([5,6,10,9]))
elem= elements.newElement("ShellMITC4",xc.ID([6,7,11,10]))
elem= elements.newElement("ShellMITC4",xc.ID([7,8,12,11]))

# active reinforcement
elements.defaultMaterial= prestressingSteel.name
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
truss.sectionAreay= Ap

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
lpSC= lPatterns.newLoadPattern("default","SC")
lpVT= lPatterns.newLoadPattern("default","VT")
lpNV= lPatterns.newLoadPattern("default","NV")
#lPatterns.currentLoadPattern= "G"
n4Load= lpG.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpG.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpG.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#lPatterns.currentLoadPattern= "SC"
n4Load= lpSC.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpSC.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpSC.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#lPatterns.currentLoadPattern= "VT"
n4Load= lpVT.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpVT.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpVT.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#lPatterns.currentLoadPattern= "NV"
n4Load= lpNV.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpNV.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpNV.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

# Combinaciones
combs= loadHandler.getLoadCombinations
comb001= combs.newLoadCombination("ELU001","1.00*G")
comb002= combs.newLoadCombination("ELU002","1.35*G")
comb003= combs.newLoadCombination("ELU003","1.00*G + 1.50*SC")
comb004= combs.newLoadCombination("ELU004","1.00*G + 1.50*SC + 0.90*NV")
comb005= combs.newLoadCombination("ELU005","1.00*G + 1.50*SC + 0.90*VT")
comb006= combs.newLoadCombination("ELU006","1.00*G + 1.50*SC + 0.90*VT + 0.90*NV")
comb007= combs.newLoadCombination("ELU007","1.00*G + 1.50*VT")
comb008= combs.newLoadCombination("ELU008","1.00*G + 1.50*VT + 0.90*NV")
comb009= combs.newLoadCombination("ELU009","1.00*G + 1.05*SC + 1.50*VT")
comb010= combs.newLoadCombination("ELU010","1.00*G + 1.05*SC + 1.50*VT + 0.90*NV")
comb011= combs.newLoadCombination("ELU011","1.00*G + 1.50*NV")
comb012= combs.newLoadCombination("ELU012","1.00*G + 0.90*VT + 1.50*NV")
comb013= combs.newLoadCombination("ELU013","1.00*G + 1.05*SC + 1.50*NV")
comb014= combs.newLoadCombination("ELU014","1.00*G + 1.05*SC + 0.90*VT + 1.50*NV")
comb015= combs.newLoadCombination("ELU015","1.35*G + 1.50*SC")
comb016= combs.newLoadCombination("ELU016","1.35*G + 1.50*SC + 0.90*NV")
comb017= combs.newLoadCombination("ELU017","1.35*G + 1.50*SC + 0.90*VT")
comb018= combs.newLoadCombination("ELU018","1.35*G + 1.50*SC + 0.90*VT + 0.90*NV")
comb019= combs.newLoadCombination("ELU019","1.35*G + 1.50*VT")
comb020= combs.newLoadCombination("ELU020","1.35*G + 1.50*VT + 0.90*NV")
comb021= combs.newLoadCombination("ELU021","1.35*G + 1.05*SC + 1.50*VT")
comb022= combs.newLoadCombination("ELU022","1.35*G + 1.05*SC + 1.50*VT + 0.90*NV")
comb023= combs.newLoadCombination("ELU023","1.35*G + 1.50*NV")
comb024= combs.newLoadCombination("ELU024","1.35*G + 0.90*VT + 1.50*NV")
comb025= combs.newLoadCombination("ELU025","1.35*G + 1.05*SC + 1.50*NV")
comb026= combs.newLoadCombination("ELU026","1.35*G + 1.05*SC + 0.90*VT + 1.50*NV")

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
#ctest.printFlag= printFlag
integ= analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([]))
soe= analysisAggregation.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","analysisAggregation","")

def solveStaticLinearComb(comb,db,dbHelp):
  preprocessor.resetLoadCase()
  dbHelp.helpSolve(comb,db)
  exec(open("solution/database_helper_solve.xci").read())
  ''' 
    print("previousName= ",previousName)
    print("tag= ",comb.tag)
    print("tagPrevia= ",tagPrevia)
    print("descomp previa= ",getDescompCombPrevia)
    print("resto sobre previa= ",getDescompRestoSobrePrevia)
  '''
  comb.addToDomain()
  analOk= analysis.analize(1)
  db.save(comb.tag*100)
  comb.removeFromDomain()


dXMin=1e9
dXMax=-1e9

def procesResultVerif(comb):
  nodes= preprocessor.getNodeHandler
  nod8= nodes.getNode(8)

  deltaX= nod8.getDisp[0] # x displacement of node 8
  global dXMin
  dXMin=min(dXMin,deltaX)
  global dXMax
  dXMax=max(dXMax,deltaX)
  ''' 
    print("tagComb= ",comb.tagComb)
    print("nmbComb= ",nmbComb)
    print("dXMin= ",(dXMin*1e3)," mm\n")
    print("dXMax= ",(dXMax*1e3)," mm\n")
   '''

import os
os.system("rm -r -f /tmp/test_combinacion_05.db")
db= feProblem.newDatabase("BerkeleyDB","/tmp/test_combinacion_05.db")

helper= dbHelper.DatabaseHelperSolve(db)

loadHandler= preprocessor.getLoadHandler
previousName="" 
tagPrevia= 0 
tagSave= 0
for key in combs.getKeys():
  comb= combs[key]
  helper.solveComb(preprocessor, comb,analysis)
  procesResultVerif(comb)

## Small differences after changing
## setSectionArea
# 2019.12.22 Values changed when initial displacements were accounted
# Maybe there is some error here.
ratio1= abs(dXMax-0.143641332276e-3)/0.143641332276e-3
ratio2= abs(dXMin+0.162147274048e-3)/0.162147274048e-3

'''
print("dXMax= ",(dXMax*1e3)," mm\n")
print("dXMin= ",(dXMin*1e3)," mm\n")
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
''' 


from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-2) & (ratio2<1e-2) :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -r -f /tmp/test_combinacion_05.db") # Your garbage you clean it
