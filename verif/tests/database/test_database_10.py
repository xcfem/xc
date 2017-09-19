# -*- coding: utf-8 -*-
# home made test
''' Check of the restore function.
   In the second analysis the solution is obtained
   at the first iteration because the state
   of the model is restored from the database. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Bar length (m)
E= 190e9 # Elastic modulus expressed in Pa
A= 140e-6 # bar area expressed in square meters
fMax= 1860e6 # Maximum unit load of the material expressed in Pa.
fy= 1171e6 # Yield stress of the material expressed in Pa.
tInic= 0.75**2*fMax # Effective prestress (0.75*P0 y 25% prestress losses).

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0.0,0.0)
nod= nodes.newNodeXY(L,0.0)


# Materials definition
mat= typical_materials.defSteel02(preprocessor, "prestressingSteel",E,fy,0.001,tInic)
    
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "prestressingSteel"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for the next element.
truss= elements.newElement("Truss",xc.ID([1,2]));
truss.area= A

    
# Constraints
constraints= preprocessor.getConstraintLoader

#
spc= constraints.newSPConstraint(1,0,0.0)
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,0,0.0)
spc= constraints.newSPConstraint(2,1,0.0)

    

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
convTest= smt.newConvergenceTest("norm_unbalance_conv_test")
convTest.tol=1.0e-9
convTest.maxNumIter= 10
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")


analysis= solu.newAnalysis("static_analysis","smt","")
result= analysis.analyze(1)

import os
os.system("rm -rf /tmp/test10.db")
db= prueba.newDatabase("BerkeleyDB","/tmp/test10.db")
db.save(100)
prueba.clearAll()
db.restore(100)


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
convTest= smt.newConvergenceTest("norm_unbalance_conv_test")
convTest.tol=1.0e-9
convTest.maxNumIter= 10
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")


analysis= solu.newAnalysis("static_analysis","smt","")
result= analysis.analyze(1)


elements= preprocessor.getElementLoader

elem1= elements.getElement(1)
elem1.getResistingForce()
ratio= (tInic*A-elem1.getN())/(tInic*A)

'''
print{"force= ",getN
print "prestressing force= ",(tInic*A)
print "strain= ",getStrain
print "ratio= ",ratio
'''
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<0.02:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -rf /tmp/test10.db") # Your garbage you clean it
