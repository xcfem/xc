# -*- coding: utf-8 -*-
# home made test
# Referencia:  Cálculo de estructuras por el método de los elementos finitos. E. Oñate, pg. 165, ejemplo 5.3

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Módulo elástico
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante
alpha= 1.2e-5 # Coeficiente de dilatación del acero
A= 4e-4 # Área de la barra expresada en metros cuadrados
Iy= 80.1e-8 # Momento de inercia de la sección expresada en m4
Iz= 8.49e-8 # Momento de inercia de la sección expresada en m4
J= 0.721e-8 # Momento de inercia a torsión expresado en m4
AT= 10 # Incremento de temperatura expresado en grados centígrados

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nodos.newNodeXYZ(0.0,0.0,0.0)
nodos.newNodeXYZ(L,0.0,0.0)


trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

scc= typical_materials.defElasticSection3d(mdlr,"scc",A,E,G,Iz,Iy,J)

# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
elementos.dimElem= 2
elementos.defaultTag= 1 #Tag for next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));
    
# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.fixNode6DOF(coacciones,2)

    # Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("linear_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([1])
defTermica= xc.PlanoDeformacion(alpha*AT)
eleLoad.planoDeformacionDorsal= defTermica
eleLoad.planoDeformacionFrontal= defTermica

#We add the load case to domain.
casos.addToDomain("0")

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

elementos.getElement(1).getResistingForce()
axil1= elementos.getElement(1).getN1
axil2= elementos.getElement(1).getN2

N= (-E*A*alpha*AT)
ratio= ((axil2-N)/N)

# print "N= ",N
# print "axil1= ",axil1
# print "axil2= ",axil2
# print "ratio= ",ratio


import os
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
