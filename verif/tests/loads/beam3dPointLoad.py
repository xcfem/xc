# -*- coding: utf-8 -*-
# home made test
# Referencia:  Expresiones de la flecha el el Prontuario de
# Estructuras Metálicas del CEDEX. Apartado 3.3 Carga puntual sobre ménsula.}

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2e6 # Módulo elástico
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante
L= 20 # Longitud de la barra.
h= 0.30 # Canto de la sección.
b= 0.2 # Ancho de la sección.
A= b*h # Área de la sección.
Iz= b*h**3/12 # Inercia de la sección
Iy= h*b**3/12 # Inercia de la sección
J= 1e-8 # Inercia a torsión de la sección
x= 0.5 # Abcisa relativa en la que se aplica la carga puntual.
P= 1e3 # Carga puntual.
n= 1e6 # Carga uniforme axial.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales3D(nodos)

nodos.defaultTag= 1 #First node number.
nodos.newNodeXYZ(0,0,0)
nodos.newNodeXYZ(L,0,0)
    
# Definimos transformaciones geométricas
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,0,1])
    
# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
elementos.defaultTag= 1 #Tag for next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));
    
# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)

 # Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam3d_point_load")
eleLoad.elementTags= xc.ID([1])
eleLoad.axialComponent= n
eleLoad.transComponent= -P
eleLoad.x= x
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

delta0= nodos.getNode(2).getDisp[0] #x displacement of node 2.
delta1= nodos.getNode(2).getDisp[1] #y displacement of node 2.

a= x*L
delta0Teor= (n*a/E/A)
ratio0= ((delta0-delta0Teor)/delta0Teor)
delta1Teor= (-P*a**2*(3*L-a)/6/E/Iz)
ratio1= ((delta1-delta1Teor)/delta1Teor)


# print "delta0= ",delta0
# print "delta0Teor= ",delta0Teor
# print "ratio0= ",ratio0
# print "delta1= ",delta1
# print "delta1Teor= ",delta1Teor
# print "ratio1= ",ratio1

import os
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-10) & (abs(ratio1)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
