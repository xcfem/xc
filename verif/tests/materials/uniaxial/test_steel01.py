# -*- coding: utf-8 -*-
# home made test

fy= 2600 # Tensión de cedencia del acero.
E= 2.1e6 # Módulo de Young del acero.
l= 1 # Distancia entre nodos
epsy= fy/E # Deformación para la que se produce la cedencia
D= 1.5*epsy # Magnitud del desplazamiento impuesto
F= 1.05*E*epsy # Fuerza a aplicar.
Nsteps= 10 # Número de pasos para el análisis.


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

# Puntos de la función tensión - deformación
x_modelo= [0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016,0.0014,0.0012,0.001,0.0008,0.0006,0.0004,0.0002,8.13152e-20,-0.0002,-0.0004,-0.0006,-0.0008,-0.001,-0.0012,-0.0014,-0.0016,-0.0014,-0.0012,-0.001,-0.0008,-0.0006,-0.0004,-0.0002,-8.13152e-20,0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016]
y_modelo= [420,840,1260,1680,2100,2520,2600.34,2600.76,2180.76,1760.76,1340.76,920.76,500.76,80.76,-339.24,-759.24,-1179.24,-1599.24,-2019.24,-2439.24,-2599.5,-2599.92,-2600.34,-2600.76,-2180.76,-1760.76,-1340.76,-920.76,-500.76,-80.76,339.24,759.24,1179.24,1599.24,2019.24,2439.24,2599.5,2599.92,2600.34,2600.76]


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
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)
    
''' Se definen nodos en los puntos de aplicación de
    la carga. Puesto que no se van a determinar tensiones
    se emplea una sección arbitraria de área unidad '''
    
# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "acero"
elementos.dimElem= 2
elementos.defaultTag= 1 #Tag for the next element.
muelle= elementos.newElement("muelle",xc.ID([1,2]));

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

''' 
\prop_recorder

nodos= preprocessor.getNodeLoader{2}
            \callback_record

                
d= .getDisp[0]
print (d*1000)

\callback_restart{print("Se llamó al método restart."}



'''
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

integ.dU1= -0.0002 #Unload
result= analysis.analyze(16)

integ.dU1= 0.0002 #Reload
result= analysis.analyze(16)

nodos.calculateNodalReactions(True)
nodos= preprocessor.getNodeLoader
nod2= nodos.getNode(2)
deltax= nod2.getDisp[0] 
deltay= nod2.getDisp[1] 
nod1= nodos.getNode(1)
R= nod1.getReaction[0] 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
Ax= elem1.getMaterial().getStrain() # Alargamiento del muelle



ratio1= abs((F+R)/F)
ratio2= abs((deltax-Ax)/Ax)
#resta= ley-ley_modelo
resta_x= []
resta_y= []
def substract(x,y): return x-y
resta_x= map(substract,x,x_modelo)
resta_y= map(substract,y,y_modelo)

ratio3= 0
for d in resta_x:
  ratio3= ratio3+d**2
ratio3= math.sqrt(ratio3)
ratio4= 0
for d in resta_y:
  ratio4= ratio4+d**2
ratio4= math.sqrt(ratio4)

''' 
print "R= ",R
print "dx= ",deltax
print "dy= ",deltay
print "Ax= ",Ax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "x= ",x
print "resta_x= ",resta_x
print "ratio3= ",ratio3
print "y= ",y
print "y_modelo= ",y_modelo
print "resta_y= ",resta_y
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if((ratio3<1e-12) & (ratio4<1e-12)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
