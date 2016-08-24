# -*- coding: utf-8 -*-
# home made test
#    Prueba del funcionamiento de la base de datos
#    como almacén de combinaciones para acelerar el cálculo.

Ec= 2e5*9.81/1e-4 # Módulo de Young del hormigón en Pa.
nuC= 0.2 # Poisson's ratio del hormigón EHE-08.
hLosa= 0.2 # Espesor.
densLosa= 2500*hLosa # Densidad de la losa kg/m2.
# Load
F= 5.5e4 # Load magnitude en N

# Armadura activa
Ep= 190e9 # Elastic modulus expresado en MPa
Ap= 140e-6 # Área de la barra expresada en metros cuadrados
fMax= 1860e6 # Carga unitaria máxima del material expresada en MPa.
fy= 1171e6 # Tensión de límite elástico del material expresada en Pa.
tInic= 0.75**2*fMax # Pretensado final (incial al 75 por ciento y 25 por ciento de pérdidas totales).

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from solution import database_helper as dbHelper

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)
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

typical_materials.defSteel02(preprocessor, "aceroPret",Ep,fy,0.001,tInic)

elementos= preprocessor.getElementLoader
# Losa de hormigón
elementos.defaultMaterial= "hLosa"
elementos.defaultTag= 1
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,6,5]))

elem= elementos.newElement("shell_mitc4",xc.ID([2,3,7,6]))
elem= elementos.newElement("shell_mitc4",xc.ID([3,4,8,7]))
elem= elementos.newElement("shell_mitc4",xc.ID([5,6,10,9]))
elem= elementos.newElement("shell_mitc4",xc.ID([6,7,11,10]))
elem= elementos.newElement("shell_mitc4",xc.ID([7,8,12,11]))

# Armadura activa
elementos.defaultMaterial= "aceroPret"
elementos.dimElem= 3
truss= elementos.newElement("truss",xc.ID([1,2]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([2,3]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([3,4]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([5,6]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([6,7]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([7,8]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([9,10]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([10,11]));
truss.area= Ap
truss= elementos.newElement("truss",xc.ID([11,12]));
truss.area= Ap

# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.fixNode6DOF(coacciones,5)
fix_node_6dof.fixNode6DOF(coacciones,9)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"

lpG= casos.newLoadPattern("default","G")
lpSC= casos.newLoadPattern("default","SC")
lpVT= casos.newLoadPattern("default","VT")
lpNV= casos.newLoadPattern("default","NV")
#casos.currentLoadPattern= "G"
n4Load= lpG.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpG.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpG.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#casos.currentLoadPattern= "SC"
n4Load= lpSC.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpSC.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpSC.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#casos.currentLoadPattern= "VT"
n4Load= lpVT.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpVT.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpVT.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

#casos.currentLoadPattern= "NV"
n4Load= lpNV.newNodalLoad(4,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n8Load= lpNV.newNodalLoad(8,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
n12Load= lpNV.newNodalLoad(12,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

# Combinaciones
combs= cargas.getLoadCombinations
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

solu= prueba.getSoluProc
solCtrl= solu.getSoluControl


solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")


cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")

solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= smt.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-3
ctest.maxNumIter= 10
#ctest.printFlag= printFlag
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","smt","")



def resuelveCombEstatLin(comb,db,dbHelp):
  preprocessor.resetLoadCase()
  dbHelp.helpSolve(comb,db)
  execfile("solution/database_helper_solve.xci")
  ''' 
    print "nombrePrevia= ",nombrePrevia
    print "tag= ",comb.tag
    print "tagPrevia= ",tagPrevia
    print "descomp previa= ",getDescompCombPrevia
    print "resto sobre previa= ",getDescompRestoSobrePrevia
  '''
  comb.addToDomain()
  analOk= analysis.analize(1)
  db.save(comb.tag*100)
  comb.removeFromDomain()


dXMin=1e9
dXMax=-1e9

def procesResultVerif(comb):
  nodes= preprocessor.getNodeLoader
  nod8= nodes.getNode(8)

  deltaX= nod8.getDisp[0] # Desplazamiento del node 2 según z
  global dXMin
  dXMin=min(dXMin,deltaX)
  global dXMax
  dXMax=max(dXMax,deltaX)
  ''' 
    print "tagComb= ",comb.tagComb
    print "nmbComb= ",nmbComb
    print "dXMin= ",(dXMin*1e3)," mm\n"
    print "dXMax= ",(dXMax*1e3)," mm\n"
   '''

import os
os.system("rm -r -f /tmp/test_combinacion_05.db")
db= prueba.newDatabase("BerkeleyDB","/tmp/test_combinacion_05.db")

helper= dbHelper.DatabaseHelperSolve(db)

cargas= preprocessor.getLoadLoader
nombrePrevia="" 
tagPrevia= 0 
tagSave= 0
for key in combs.getKeys():
  comb= combs[key]
  helper.solveComb(preprocessor, comb,analysis)
  procesResultVerif(comb)

ratio1= abs((dXMax-0.115734e-3)/0.115734e-3)
ratio2= abs((dXMin+0.0872328e-3)/0.0872328e-3)

''' 
print "dXMax= ",(dXMax*1e3)," mm\n"
print "dXMin= ",(dXMin*1e3)," mm\n"
print "ratio1= ",ratio1
print "ratio2= ",ratio2
'''

fname= os.path.basename(__file__)
if (ratio1<1e-5) & (ratio2<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

os.system("rm -r -f /tmp/test_combinacion_05.db") # Your garbage you clean it
