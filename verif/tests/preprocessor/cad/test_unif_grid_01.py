# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

ndivZ= 500

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)
# Definimos materiales
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodos.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]));
truss.area= 10.0

unifGrids= preprocessor.getCad.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 1
uGrid.Ly= 1
uGrid.Lz= 10
uGrid.nDivX= 0
uGrid.nDivY= 0
uGrid.nDivZ= ndivZ

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodos= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodosTeor= ndivZ+1
numElemTeor= ndivZ

import os
fname= os.path.basename(__file__)
if (abs(numNodosTeor-numNodos)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
