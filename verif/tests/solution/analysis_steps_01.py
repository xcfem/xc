# -*- coding: utf-8 -*-
# home made test

E= 2.1e9 # Módulo de Young del acero.
l= 1 # Distancia entre nodos
Nsteps= 10 # Número de pasos para el análisis.
F= 1e3

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

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_elasticidad2D(nodos)


nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0)
nod= nodos.newNodeXY(l,0.0)

# Materials definition
elast0= typical_materials.defElasticMaterial(preprocessor, "elast0",E)
    
''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "elast0"
elementos.dimElem= 2
elementos.defaultTag= 1 #Tag for the next element.
spring= elementos.newElement("spring",xc.ID([1,2]));

# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(2,1,0.0) # Nodo 2

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#modulación de la carga en el tiempo:
ts= casos.newTimeSeries("trig_ts","ts")
ts.factor= 1
ts.tStart= 0
ts.tFinish= 2
ts.period= 1
ts.shift= 0
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0]))

#We add the load case to domain.
casos.addToDomain("0")

x= []
y= []
recorder= prueba.getDomain.newRecorder("element_prop_recorder",None);
recorder.setElements(xc.ID([1]))
recorder.callbackRecord= "x.append(self.getMaterial().getStrain()); y.append(self.getN())"
recorder.callbackRestart= "print \"Restart method called.\""


# Procedimiento de solución
solu= prueba.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("ldctrl","sm")
solAlgo= smt.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= smt.newConvergenceTest("energy_inc_conv_test")
ctest.tol= 1e-9
ctest.maxNumIter= 10
ctest.printFlag= 0
integ= smt.newIntegrator("displacement_control_integrator",xc.Vector([]))
integ.nod= 2
integ.dof= 0
integ.dU1= 0.0002
soe= smt.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","ldctrl","")
result= analysis.analyze(8)

