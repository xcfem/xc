# -*- coding: utf-8 -*-
# Test tomado del manual de Ansys
# Referencia:  Strength of Material, Part I, Elementary Theory and Problems, pg. 26, problem 10

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from model import fija_nodos_lineas
from model import cargas_nodo
from materials import typical_materials

NumDiv= 100
E= 30e6 # Young modulus (psi)
lng= 10 # Longitud del cable en pulgadas
sigmaPret= 1500 # Magnitud del pretensado en libras
area= 2.0
fPret= sigmaPret*area # Magnitud del pretensado en libras
F= 100/NumDiv # Carga vertical

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)

# Definición del modelo
nodos.newSeedNode()

    
# Definimos materiales
typical_materials.defCableMaterial(mdlr,"cable",E,sigmaPret,0.0)
    
''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Definimos elemento semilla
seedElemLoader= mdlr.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "cable"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #El número del próximo elemento será 1.
truss= seedElemLoader.newElement("corot_truss",xc.ID([1,2]))
truss.area= area
# fin de la definición del elemento semilla

puntos= mdlr.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(lng,0.0,0.0))
lines= mdlr.getCad.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv

l1= mdlr.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)
    
# Constraints
coacciones= mdlr.getConstraintLoader
fija_nodos_lineas.CondContornoNodosExtremosLinea(l,coacciones,fix_node_6dof.fixNode6DOF)
fija_nodos_lineas.CondContornoNodosInterioresLinea(l,coacciones,fix_node_6dof.Nodo6DOFGirosImpedidos)

# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
lPattern= "0"
lp0= casos.newLoadPattern("default",lPattern)
casos.currentLoadPattern= lPattern
cargas_nodo.CargaNodosInterioresLinea(l1,lp0,xc.Vector([0,-F,0,0,0,0]))
casos.addToDomain(lPattern)


Nstep= 10  #  apply load in 10 steps
DInc= 1./Nstep 	#  first load increment


solu= prueba.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
cHandler= sm.newConstraintHandler("plain_handler")
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= smt.newConvergenceTest("norm_unbalance_conv_test")
ctest.tol= 1e-6
ctest.maxNumIter= 100
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
integ.dLambda1= DInc
soe= smt.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","smt","")
result= analysis.analyze(Nstep)


tagN1= l.firstNode.tag
tagN2= l.lastNode.tag
index= int(NumDiv/2)+1
tagN3= l.getNodeI(index).tag



nodos.calculateNodalReactions(True)
nodos= mdlr.getNodeLoader
R1X= nodos.getNode(tagN2).getReaction[0]
R1Y= nodos.getNode(tagN2).getReaction[1] 
R2X= nodos.getNode(tagN1).getReaction[0]
R2Y= nodos.getNode(tagN1).getReaction[1] 
deltaX= nodos.getNode(tagN3).getDisp[0]
deltaY= nodos.getNode(tagN3).getDisp[1]  


alpha= -math.atan2(deltaY,lng/2)
Ftot= (NumDiv-1)*F
ratio1= (abs(R1X+R2X)/fPret)
ratio2= (abs(R1Y-Ftot/2.0)/(Ftot/2))
ratio3= (abs(R2Y-Ftot/2.0)/(Ftot/2))
    
''' 
print "tagN1= ",tagN1
print "tagN2= ",tagN2
print "tagN3= ",tagN3
print "F= ",(F)
print "alpha= ",rad2deg((alpha))
print "R1X= ",R1X
print "R1Y= ",R1Y
print "R2X= ",R2X
print "R2Y= ",R2Y
print "deltaX= ",deltaX
print "deltaY= ",deltaY
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
'''
    
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11) & (abs(ratio2)<1e-11) & (abs(ratio3)<1e-11) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

