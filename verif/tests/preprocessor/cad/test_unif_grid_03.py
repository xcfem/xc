# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

ndivX= 5
ndivY= 8
ndivZ= 2

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Definimos materiales
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",30e6,0.3,0.0)

nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodes)


nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast3d"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
brick= seedElemLoader.newElement("brick",xc.ID([0,0,0,0,0,0,0,0]));

unifGrids= preprocessor.getCad.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 10
uGrid.Ly= 10
uGrid.Lz= 10
uGrid.nDivX= ndivX
uGrid.nDivY= ndivY
uGrid.nDivZ= ndivZ


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodesTeor= (ndivX+1)*(ndivY+1)*(ndivZ+1)
numElemTeor= ndivX*ndivY*ndivZ

import os
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
