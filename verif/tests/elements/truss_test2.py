# -*- coding: utf-8 -*-

# Test tomado del manual de Ansys
# Referencia:  Strength of Material, Part I, Elementary Theory and Problems, pg. 10, problem 2

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import math


E= 30e6 #Young modulus (psi)
l= 15*12 #Longitud de la barra (15 pies) expresada en pulgadas}
theta= math.radians(30) #angle between bars
F= 5000 #Magnitud de la fuerza en libras}
A= 0.5 #Área en pulgadas cuadradas}
a= 2*l*math.cos(theta) #Distancia entre nodos extremos}
b= l*math.sin(theta) #Distancia entre nodos extremos}

prb= xc.ProblemaEF()
mdlr= prb.getModelador
nodos= mdlr.getNodeLoader

# Problem type
predefined_spaces.gdls_elasticidad2D(nodos)

nodos.defaultTag= 1 #First node number.
nodos.newNodeXYZ(0,0,0)
nodos.newNodeXYZ(a/2,-b,0)
nodos.newNodeXYZ(a,0,0)

# Materials definition
elast= typical_materials.defElasticMaterial(mdlr,"elast",E)
elast.E= E


# Element definition.
elementos= mdlr.getElementLoader
elementos.dimElem= 2 #Bidimensional space.
elementos.defaultMaterial= "elast"
elementos.defaultTag= 1 #Next element number.
truss= elementos.newElement("truss",xc.ID([1,2]));
truss.area= A
truss= elementos.newElement("truss",xc.ID([2,3]))
truss.area= A

coacciones= mdlr.getConstraintLoader
#Zerp movement for node 1.
spc= coacciones.newSPConstraint(1,0,0.0)
spc= coacciones.newSPConstraint(1,1,0.0)
#Zerp movement for node 1.
spc= coacciones.newSPConstraint(3,0,0.0)
spc= coacciones.newSPConstraint(3,1,0.0)

cargas= mdlr.getLoadLoader
#Load case container:
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prb)
result= analisis.analyze(1)

delta= nodos.getNode(2).getDisp[1]
stress= elementos.getElement(1).getMaterial().getStress()

ratio1= delta/(-0.12)
ratio2= stress/10000

import os
fname= os.path.basename(__file__)
if abs(ratio1-1)<1e-5 and abs(ratio2-1)<1e-5:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

