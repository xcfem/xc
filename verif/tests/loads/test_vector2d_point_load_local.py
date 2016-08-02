# -*- coding: utf-8 -*-
# home made test
# Reference:  Expresiones de la flecha el el Prontuario de
# Estructuras Metálicas del CEDEX. Apartado 3.3 Carga puntual sobre ménsula.
# Comprobamos el funcionamient del comando vector2d_point_load_local.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

E= 2e6 # Elastic modulus
L= 20 # Bar length.
h= 0.30 # Canto de la viga.
b= 0.2 # Ancho de la viga.
A= b*h # Cross section area.
I= b*h**3/12 # Inercia de la viga en pulgadas a la cuarta
x= 0.5 # Abcisa relativa en la que se aplica la carga puntual.
P= 1e3 # Carga transversal.
n= 1e6 # Carga axial.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0)
nod= nodos.newNodeXY(L,0.0)

    
# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")

    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
elementos.defaultTag= 1 #Tag for next element.
beam2d= elementos.newElement("elastic_beam_2d",xc.ID([1,2]))
beam2d.h= h

# fin de la definición de elementos
    
# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
casos.currentLoadPattern= "0"

mesh= prueba.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  elem.vector2dPointByRelDistLoadLocal(x,xc.Vector([n,-P]))
  elem= eIter.next()

cargas= preprocessor.getLoadLoader
#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

nod2= nodos.getNode(2)
delta0= nod2.getDisp[0]
delta1= nod2.getDisp[1]

a= x*L
delta0Teor= (n*a/E/A)
ratio0= ((delta0-delta0Teor)/delta0Teor)
delta1Teor= (-P*a**2*(3*L-a)/6/E/I)
ratio1= ((delta1-delta1Teor)/delta1Teor)


# print "delta0= ",delta0
# print "delta0Teor= ",delta0Teor
# print "ratio0= ",ratio0
# print "delta1= ",delta1
# print "delta1Teor= ",delta1Teor
# print "ratio1= ",ratio1


import os
fname= os.path.basename(__file__)
if ((abs(ratio0)<1e-10) & (abs(ratio1)<1e-12)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
