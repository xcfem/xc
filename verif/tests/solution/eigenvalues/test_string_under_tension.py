# -*- coding: utf-8 -*-
''' Test basado en el denominado VM53 del manual de verificación de Ansys
   Reference: Thomson, Ref 9: Vibration Theory and Applications, pg. 264, article 8.2.
   '''
from __future__ import division
import xc_base
import geom
import xc

from model import fix_node_3dof
from model import fija_nodos_lineas
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

NumDiv= 13
E= 30e6 # Young modulus (psi)
l= 100.0 # Longitud de la cuerda in inches
sigmaPret= E*0.00543228 # Prestressing force (pounds)
area= 0.00306796 # Area de la sección in inches cuadradas
Mass= 0.00073*area # Masa por unidad de longitud.
MassNod= Mass*(l/NumDiv) # Masa por unidad de longitud.
fPret= sigmaPret*area # Prestressing force (pounds)


# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
# Problem type
predefined_spaces.gdls_resist_materiales2D(nodes)
nodes.newSeedNode()

# Materials definition
typical_materials.defCableMaterial(preprocessor, "cable",E,sigmaPret,Mass)

''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Definimos elemento semilla
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "cable"
seedElemLoader.dimElem= 2
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("corot_truss",xc.ID([0,0]))
truss.area= area
# fin de la definición del elemento semilla

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(l,0.0,0.0))
lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)
    
# Constraints
coacciones= preprocessor.getConstraintLoader
fija_nodos_lineas.CondContornoNodosExtremosLinea(l1,coacciones,fix_node_3dof.fixNode000)
fija_nodos_lineas.CondContornoNodosInterioresLinea(l1,coacciones,fix_node_3dof.fixNodeFF0)


Nstep= 10  #  apply load in 10 steps
DInc= 1./Nstep 	#  first load increment
solu= prueba.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("plain_handler")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= smt.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-8
ctest.maxNumIter= 100
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
integ.dLambda1= DInc
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","smt","")
result= analysis.analyze(Nstep)

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
traccion= ele1.getN()
sigma= ele1.getMaterial().getStress()


# Procedimiento de solución

#solu.clearAll()
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("frequency_soln_algo")
integ= smt.newIntegrator("eigen_integrator",xc.Vector([1.0,1,1.0,1.0]))
soe= smt.newSystemOfEqn("sym_band_eigen_soe")
solver= soe.newSolver("sym_band_eigen_solver")
analysis= solu.newAnalysis("eigen_analysis","smt","")


analOk= analysis.analyze(3)
eig1= analysis.getEigenvalue(1)
eig2= analysis.getEigenvalue(2)
eig3= analysis.getEigenvalue(3)
f1= math.sqrt(eig1)/(2*math.pi)
f2= math.sqrt(eig2)/(2*math.pi)
f3= math.sqrt(eig3)/(2*math.pi)



ratio1= abs((sigma-162974)/162974)
ratio2= abs((traccion-500)/500)

ratio3= abs((f1-74.708)/74.708)
ratio4= abs((f2-149.42)/149.42)
ratio5= abs((f3-224.12)/224.12)

''' 
print "tensión= ",sigma
print "ratio1= ",(ratio1)
print "traccion= ",traccion
print "ratio2= ",(ratio2)

print "eig1= ",eig1
print "eig2= ",eig2
print "eig3= ",eig3
print "f1= ",math.sqrt(eig1)/(2*math.pi)
print "ratio3= ",(ratio3)
print "f2= ",math.sqrt(eig2)/(2*math.pi)
print "ratio4= ",(ratio4)
print "f3= ",math.sqrt(eig3)/(2*math.pi)
print "ratio5= ",(ratio5)
   '''
 
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-2) & (abs(ratio3)<1e-2) & (abs(ratio5)<5e-2) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
  
