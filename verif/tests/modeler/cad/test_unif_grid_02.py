# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

# nverborrea= 0

ndivX= 5
ndivY= 8

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",30e6,0.3,0.0)

nodos= preprocessor.getNodeLoader

nodos.dimEspace= 3 # Dimensiones de los nodos
nodos.numGdls= 2 # Grados de libertad de los nodos
nodos.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast2d"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
quad4n= seedElemLoader.newElement("quad4n",xc.ID([0,0,0,0]));

unifGrids= preprocessor.getCad.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 10
uGrid.Ly= 10
uGrid.Lz= 1
uGrid.nDivX= ndivX
uGrid.nDivY= ndivY
uGrid.nDivZ= 0

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodos= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodosTeor= (ndivX+1)*(ndivY+1)
numElemTeor= ndivX*ndivY

import os
fname= os.path.basename(__file__)
if (abs(numNodosTeor-numNodos)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
