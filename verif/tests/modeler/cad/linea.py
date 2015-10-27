# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import sys
import math

NumDiv= 25
CooMax= 10.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_elasticidad3D(nodos)

# Material
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodos.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.dimElem= 3 #Las barras se definen e un espacio tridimensional.
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.defaultTag= 1 #El número del próximo elemento será 1.
truss= seedElemLoader.newElement("truss",xc.ID([1,2]));
truss.area= 10

puntos= preprocessor.getCad.getPoints
puntos.defaultTag= 1
pt= puntos.newPntFromPos3d(geom.Pos3d(0,0,0))
pt= puntos.newPntFromPos3d(geom.Pos3d(CooMax,CooMax,CooMax))

lineas= preprocessor.getCad.getLines
lineas.defaultTag= 1
l= lineas.newLine(1,2)
l.nDiv= NumDiv

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

mesh= prueba.getDomain.getMesh
nnod= mesh.getNumNodes()
nelem= mesh.getNumElements()

# nIter= mesh.getNodeIter
# nod= nIter.next()
# while not(nod is None):
#   print "tag= ", nod.tag, " x= ", nod.getCoo[0], " y= ", nod.getCoo[1], " z= ", nod.getCoo[2]
#   nod= nIter.next()

vteor2= math.pow(CooMax/NumDiv,2)
lteor= math.sqrt(3*vteor2)

cumple= True
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
#  print elem.tag," nod. I: ",elem.getNodes[0].tag," nod. J: ",elem.getNodes[1].tag," L= ",elem.getL()
  ratio1= (lteor - elem.getL())/lteor
  cumple= cumple and (ratio1<1e-12)
  elem= eIter.next()

#print "núm. nodos: ",nnod
#print "núm. elementos: ",nelem

import os
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."



