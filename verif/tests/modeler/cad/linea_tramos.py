# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

NumDiv= 10
CooMax= 10
raiz3= math.sqrt(3)

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)
# Materials definition
elast= typical_materials.defElasticMaterial(mdlr,"elast",3000)

nodos.newSeedNode()
seedElemLoader= mdlr.getElementLoader.seedElemLoader
seedElemLoader.dimElem= 3 #Las barras se definen e un espacio tridimensional.
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.defaultTag= 1 #El número del próximo elemento será 1.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]))
truss.area= 10

puntos= mdlr.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))

lines= mdlr.getCad.getLines
lines.defaultTag= 1
l= lines.newDividedLine(1,2)
l.setLongs(xc.Vector([raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3]))

setTotal= mdlr.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

''' 
print "núm. nodos: ",nnod
nodos= mdlr.getNodeLoader
for_each
  print "  nodo: ",tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]

print "núm. elementos: ",nelem
'''
elementos= setTotal.getElements
cumple= 1
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elementos:
  # print "  elem: ",tag," nod. I: ",nod[0].tag," nod. J: ",nod[1].tag," L= ",length
  # print "lteor: ",(lteor)
  ratio1= (lteor/e.getL())
  cumple= (abs(ratio1-1.0)<1e-5) & (cumple) 
  # print "cumple: ",(cumple)

import os
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
