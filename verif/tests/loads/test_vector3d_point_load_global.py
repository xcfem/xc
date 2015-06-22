from __future__ import division
# -*- coding: utf-8 -*-
# home made test
# Referencia:  Expresiones de la flecha el el Prontuario de
# Estructuras Metálicas del CEDEX. Apartado 3.3 Carga puntual sobre ménsula.
# Comprobamos el funcionamient del comando vector3d_point_load_local.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
import math


E= 2e6 # Módulo elástico
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante
L= 20 # Longitud de la barra.
h= 0.30 # Canto de la sección.
b= 0.20 # Ancho de la sección.
A= b*h # Área de la sección.
Iz= b*h**3/12.0 # Inercia de la sección
Iy= h*b**3/12.0 # Inercia de la sección
J= 1e-8 # Inercia a torsión de la sección
x= 0.5 # Abcisa relativa en la que se aplica la carga puntual.
P= 1e3 # Carga puntual transversal.
n= 1e6 # Carga puntual axial.

ptoAplic= xc.Vector([1+x*L*math.sqrt(2)/2.0,3+x*L*math.sqrt(2)/2.0,0])

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)

nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(1,3,0)
nod= nodos.newNodeXYZ(1+L*math.sqrt(2)/2.0,3+L*math.sqrt(2)/2.0,0)

# Definimos transformaciones geométricas
trfs= mdlr.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,0,1])
    
# Materials definition
scc= typical_materials.defElasticSection3d(mdlr,"scc",A,E,G,Iz,Iy,J)


# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "scc"
elementos.defaultTag= 1 #Tag for next element.
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));

    
# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)


# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
casos.currentLoadPattern= "0"

vIElem= xc.Vector([1,0,0])
vJElem= xc.Vector([0,1,0])
mesh= prueba.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  crdTransf= elem.getCoordTransf
  vIElem= crdTransf.getIVector
  vJElem= crdTransf.getJVector
  vCarga= n*vIElem-P*vJElem
  elem.vector3dPointLoadGlobal(ptoAplic,vCarga)
  elem= eIter.next()

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodos= mdlr.getNodeLoader
nod2= nodos.getNode(2)
tmp= nod2.getDisp
vDispUVW= xc.Vector([tmp[0],tmp[1],tmp[2]])
a= x*L
delta0= vDispUVW.dot(vIElem)
delta0Teor= (n*a/E/A)
ratio0= ((delta0-delta0Teor)/delta0Teor)
delta1= vDispUVW.dot(vJElem)
delta1Teor= (-P*a**2*(3*L-a)/6.0/E/Iz)
ratio1= ((delta1-delta1Teor)/delta1Teor)

''' 
print "vDispUVW= ",vDispUVW
print "vIElem= ",vIElem
print "vJElem= ",vJElem
print "delta0= ",delta0
print "delta0Teor= ",delta0Teor
print "ratio0= ",ratio0
print "delta1= ",delta1
print "delta1Teor= ",delta1Teor
print "ratio1= ",ratio1
 '''

import os
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-10) & (abs(ratio1)<1e-11):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
