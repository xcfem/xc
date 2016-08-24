# -*- coding: utf-8 -*-
# Test tomado del ejemplo 9-1 del libro: Introducción Al Estudio Del Elemento Finito en Ingeniería. Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. Pearson Educación, 1999
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",200000,0.3,0.0)

nodes= preprocessor.getNodeLoader 
predefined_spaces.gdls_elasticidad3D(nodes)
nodes.defaultTag= 1 #Next node number.
nod1= nodes.newNodeXYZ(100,0,100)
nod2= nodes.newNodeXYZ(0,0,100)
nod3= nodes.newNodeXYZ(0,0,200)
nod4= nodes.newNodeXYZ(100,0,200)
nod5= nodes.newNodeXYZ(100,100,100)
nod6= nodes.newNodeXYZ(0,100,100)
nod7= nodes.newNodeXYZ(0,100,200)
nod8= nodes.newNodeXYZ(100,100,200)

nod9= nodes.newNodeXYZ(100,200,100)
nod10= nodes.newNodeXYZ(0,200,100)
nod11= nodes.newNodeXYZ(0,200,200)
nod12= nodes.newNodeXYZ(100,200,200)
nod13= nodes.newNodeXYZ(100,300,100)
nod14= nodes.newNodeXYZ(0,300,100)
nod15= nodes.newNodeXYZ(0,300,200)

nod16= nodes.newNodeXYZ(100,300,200)
nod17= nodes.newNodeXYZ(100,200,0)
nod18= nodes.newNodeXYZ(100,300,0)
nod19= nodes.newNodeXYZ(0,300,0)
nod20= nodes.newNodeXYZ(0,200,0)

elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "elast3d"
elementos.defaultTag= 1 #Tag for the next element.

brick1= elementos.newElement("brick",xc.ID([1,2,3,4,5,6,7,8]))
brick2= elementos.newElement("brick",xc.ID([5,6,7,8,9,10,11,12]))
brick3= elementos.newElement("brick",xc.ID([9,10,11,12,13,14,15,16]))
brick4= elementos.newElement("brick",xc.ID([9,10,14,13,17,20,19,18]))

nod17.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod18.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod19.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))
nod20.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]))

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(4,xc.Vector([0,0,-80000]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

dN1Teor= xc.Vector([-2.1569e-2,-3.7891e-3,-4.0982e-1])
nodes= preprocessor.getNodeLoader
dN1= nodes.getNode(1).getDisp

ratio= (dN1-dN1Teor).Norm()

''' 
print "dN1= ",dN1
print "dN1Teor= ",dN1Teor
print "rario= ",ratio
   '''

import os
fname= os.path.basename(__file__)
if(ratio<1e-4):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
