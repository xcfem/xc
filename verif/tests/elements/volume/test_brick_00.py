# -*- coding: utf-8 -*-
# Test tomado del ejemplo 5-001 del manual de verificación de SAP2000.
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Materials definition
elast= typical_materials.defElasticIsotropic3d(mdlr,"elast3d",1e6,0.25,0.0)

nodos= mdlr.getNodeLoader 
predefined_spaces.gdls_elasticidad3D(nodos)
nod9= nodos.newNodeIDXYZ(9,0,0,0)
nod10= nodos.newNodeIDXYZ(10,1,0,0)
nod11= nodos.newNodeIDXYZ(11,1,1,0)
nod12= nodos.newNodeIDXYZ(12,0,1,0)
nod13= nodos.newNodeIDXYZ(13,0,0,1)
nod14= nodos.newNodeIDXYZ(14,1,0,1)
nod15= nodos.newNodeIDXYZ(15,1,1,1)
nod16= nodos.newNodeIDXYZ(16,0,1,1)


elementos= mdlr.getElementLoader
elementos.defaultMaterial= "elast3d"
elementos.defaultTag= 1 #Tag for the next element.
brick= elementos.newElement("brick",xc.ID([9,10,11,12,13,14,15,16]));

coacciones= mdlr.getConstraintLoader
#Impedimos el movimiento del nodo 1.

nod9.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod10.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod11.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod12.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
lp0.newNodalLoad(13, xc.Vector([0,0,-1]))
lp0.newNodalLoad(14, xc.Vector([0,0,-1]))
lp0.newNodalLoad(15, xc.Vector([0,0,-1]))
lp0.newNodalLoad(16, xc.Vector([0,0,-1]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

nodos.calculateNodalReactions(True)
nodos= mdlr.getNodeLoader 
R9= nod9.getReaction
R10= nod10.getReaction
R11= nod11.getReaction
R12= nod12.getReaction


R= R9+R10+R11+R12
ratio1= (R-xc.Vector([0,0,4])).Norm()

''' 
print "R9= ",R9
print "R10= ",R10
print "R11= ",R11
print "R12= ",R12
print "R= ",R
print "ratio1= ",ratio1
   '''

import os
fname= os.path.basename(__file__)
if(ratio1<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
