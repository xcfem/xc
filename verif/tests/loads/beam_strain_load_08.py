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

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Coeficiente de dilatación del acero
A= 4e-4 # Área de la barra expresada en metros cuadrados
I= (2e-2)**4/12 # Cross section moment of inertia (m4)
AT= 10 # Incremento de temperatura expresado en grados centígrados
gammaF= 1.5

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0.0,0.0)
nod= nodes.newNodeXY(L,0.0)


# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
elementos.dimElem= 2
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))

    
# Constraints
coacciones= preprocessor.getConstraintLoader

#
spc= coacciones.newSPConstraint(1,0,0.0)
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,0,0.0)
spc= coacciones.newSPConstraint(2,1,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
lp0= casos.newLoadPattern("default","0")
lp0.gammaF= gammaF
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([1])
defTermica= xc.PlanoDeformacion(alpha*AT)
eleLoad.planoDeformacionDorsal= defTermica
eleLoad.planoDeformacionFrontal= defTermica
#We add the load case to domain.
casos.addToDomain("0")

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



elem1= elementos.getElement(1)
elem1.getResistingForce()
axil1= elem1.getN1
axil2= elem1.getN2


N= (-gammaF*E*A*alpha*AT)
ratio= ((axil2-N)/N)

''' 
print "N= ",N
print "axil1= ",axil1
print "axil2= ",axil2
print "ratio= ",ratio
   '''

import os
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
