# -*- coding: utf-8 -*-
# Test para comprobar el funcionamiento de getNearestElement
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import time

numElementos= 1000
tg= 0
lapso= 0
i= 1
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   
nodos= mdlr.getNodeLoader
nodos.defaultTag= 1
for i in range(1,1001):
  n= nodos.newNodeXYZ(i,0,0)

# Materials definition
elast= typical_materials.defElasticMaterial(mdlr,"elast",2.1e6)

elementos= mdlr.getElementLoader
elementos.defaultMaterial= "elast"
elementos.dimElem= 2
elementos.defaultTag= 1 #Tag for the next element.
for i in range(1,numElementos):
  truss= elementos.newElement("truss",xc.ID([i,i+1]));
  truss.area= 1

mesh= prueba.getDomain.getMesh
# print "creados ",nnod," nodos.\n"
start_time= time.time()
tg= mesh.getNearestElement(geom.Pos3d(50.51,0,0)).tag
lapso= time.time()-start_time


''' 
print "tag elemento: ",tg
print "lapso: ",lapso
   '''

import os
fname= os.path.basename(__file__)
if((tg==50)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


