# -*- coding: utf-8 -*-

#Test tomado del manual de Ansys
#Referencia:  Strength of Materials, Part I, Elementary Theory and Problems, pg. 26, problem 10

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 30e6 #Young modulus (psi)
l= 10 #Longitud de la barra en pulgadas
a= 0.3*l #Longitud del tramo a
b= 0.3*l #Longitud del tramo b
F1= 1000 #Magnitud de la fuerza 1 en libras
F2= 1000/2 #Magnitud de la fuerza 2 en libras

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

predefined_spaces.gdls_elasticidad2D(nodos)
nodos.defaultTag= 1 #El número del próximo nodo será 1.
nodos.newNodeXYZ(0,0,0)
nodos.newNodeXYZ(0,l-a-b,0)
nodos.newNodeXYZ(0,l-a,0)
nodos.newNodeXYZ(0,l,0)

elast= typical_materials.defElasticMaterial(mdlr,"elast",E)

# Se definen nodos en los puntos de aplicación de
# la carga. Puesto que no se van a determinar tensiones
# se emplea una sección arbitraria de área unidad
elementos= mdlr.getElementLoader
elementos.dimElem= 2 #Las barras se definen e un espacio bidimensional.
elementos.defaultMaterial= "elast"
elementos.defaultTag= 1 #Tag for the next element.
truss= elementos.newElement("truss",xc.ID([1,2]));
truss.area= 1
truss= elementos.newElement("truss",xc.ID([2,3]));
truss.area= 1
truss= elementos.newElement("truss",xc.ID([3,4]));
truss.area= 1

coacciones= mdlr.getConstraintLoader
#Impedimos el movimiento del nodo 1.
spc= coacciones.newSPConstraint(1,0,0.0)
spc= coacciones.newSPConstraint(1,1,0.0)
#Impedimos el movimiento del nodo 4.
spc= coacciones.newSPConstraint(4,0,0.0)
spc= coacciones.newSPConstraint(4,1,0.0)
#Impedimos el movimiento del nodo 2 según X (gdl 0).
spc= coacciones.newSPConstraint(2,0,0.0)
#Impedimos el movimiento del nodo 3 según X (gdl 0).
spc= coacciones.newSPConstraint(3,0,0.0)

cargas= mdlr.getLoadLoader
#Contenedor de hipótesis de carga:
casos= cargas.getLoadPatterns
#modulación de la carga en el tiempo:
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-F2]))
lp0.newNodalLoad(3,xc.Vector([0,-F1]))

#Agregamos el caso de carga al dominio.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

nodos.calculateNodalReactions(True)
R1= nodos.getNode(4).getReaction[1]
R2= nodos.getNode(1).getReaction[1]

ratio1= (R1-900)/900
ratio2= (R2-600)/600

#print "R1= ",R1
#print "R2= ",R2
#print "ratio1= ",ratio1
#print "ratio2= ",ratio2

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


