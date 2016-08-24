# -*- coding: utf-8 -*-
# home made test
# Reference:  Cálculo de estructuras por el método de los elementos finitos. E. Oñate, pg. 165, ejemplo 5.3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

L= 1.0 # Lado del elemento expresado en metros
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Coeficiente de dilatación del acero
h= 2e-2
A= h*h # Área de la barra expresada en metros cuadrados
I= (h)**4/12 # Cross section moment of inertia (m4)
AT= 10 # Incremento de temperatura expresado en grados centígrados

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
nodes.dimEspace= 2 # coord. for each node (x,y).
nodes.numGdls= 6 # DOF for each node (Ux,Uy).
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0.0,0.0)
nod= nodes.newNodeXY(L,0.0)
nod= nodes.newNodeXY(L,h)
nod= nodes.newNodeXY(0,h)


# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,0.3,0.0,h)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "memb1"
elementos.defaultTag= 1
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


    
# Constraints
coacciones= preprocessor.getConstraintLoader

spc= coacciones.newSPConstraint(1,0,0.0)
spc= coacciones.newSPConstraint(2,0,0.0)
spc= coacciones.newSPConstraint(3,0,0.0)
spc= coacciones.newSPConstraint(4,0,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)
spc= coacciones.newSPConstraint(3,2,0.0)
spc= coacciones.newSPConstraint(4,2,0.0)
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(2,1,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.setStrainComp(0,0,alpha*AT) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,alpha*AT)
eleLoad.setStrainComp(2,0,alpha*AT)
eleLoad.setStrainComp(3,0,alpha*AT)

#We add the load case to domain.
casos.addToDomain("0")

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



elem1= elementos.getElement(1)
elem1.getResistingForce()
n1Medio= elem1.getMeanInternalForce("n1")
n2Medio= elem1.getMeanInternalForce("n2")
axil1= (n1Medio*h)
axil2= n2Medio

N= (-E*A*alpha*AT)
ratio1= ((axil1-N)/N)
ratio2= (axil2)

''' 
print "N= ",N
print "axil1= ",axil1
print "axil2= ",axil2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-7) & (abs(ratio2)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
