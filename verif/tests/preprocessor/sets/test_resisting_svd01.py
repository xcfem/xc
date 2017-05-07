# -*- coding: utf-8 -*-
# home made test
#    Prueba del funcionamiento del comando que calcula
#    la resultante de fuerzas al cortar un subconjunto del modelo por un plano.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Ec= 2e5*9.81/1e-4 # Young modulus del hormigón en Pa.
nuC= 0.2 # Poisson's ratio del hormigón EHE-08.
hLosa= 0.2 # Espesor.
densLosa= 2500*hLosa # Densidad de la losa kg/m2.
# Load
F= 5.5e4 # Load magnitude en N

# Armadura activa
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
from model import fix_node_6dof
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
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
constraints= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(constraints,1)
fix_node_6dof.fixNode6DOF(constraints,5)
fix_node_6dof.fixNode6DOF(constraints,9)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"

lpG= casos.newLoadPattern("default","G")
#casos.currentLoadPattern= "G"
lpG.newNodalLoad(nod4.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod8.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))
lpG.newNodalLoad(nod12.tag,xc.Vector([F,0.0,0.0,0.0,0.0,0.0]))

# Combinaciones
combs= cargas.getLoadCombinations
comb= combs.newLoadCombination("ELU001","1.00*G")

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
ctest.printFlag= printFlag
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","smt","")


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
plano= geom.Plano3d(o,p1,p2)

def procesResultVerif(comb):
  tabComb= comb.tag
  nmbComb= comb.getName
  setTotal= preprocessor.getSets.getSet("total")
  resultante= setTotal.getResistingSVD3d(plano,o,0.01,0)
  global fuerza
  fuerza= resultante.getResultante()
  global momento
  momento= resultante.getMomento()
  org= resultante.getOrg


nombrePrevia= ""
tagPrevia= 0
tagSave= 0
for key in combs.getKeys():
  comb= combs[key]
  resuelveCombEstatLin(comb)
  procesResultVerif(comb)

ratio1= (abs(fuerza.x-3*F)/3/F)
ratio2= momento.getModulo()

''' 
print "fuerza= ",fuerza/1e3," kN\n"
print "momento= ",momento/1e3," kN\n"
print "org= ",Org," m\n"
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-10) & (ratio2<1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
