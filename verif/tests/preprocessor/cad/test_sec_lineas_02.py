# -*- coding: utf-8 -*-

from __future__ import division
import xc_base
import geom
import xc
import math
import os
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 6
CooMax= 10

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.dimElem= 3 #Bars defined in a three dimensional space.
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.defaultTag= 1 #Number for the next element will be 1.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]))
truss.area= 10

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax/3,CooMax/3,CooMax/3))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(2*CooMax/3,2*CooMax/3,2*CooMax/3))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getCad.getLines
lines.defaultTag= 3
l3= lines.newLine(1,2)        

# print "nombre: ",nombre," p1:",p1.nombre," p2:",p2.nombre
lines.defaultTag= 2
l2= lines.newLine(4,3)
# print "nombre: ",nombre," p1:",p1.nombre," p2:",p2.nombre
lines.defaultTag= 1
l1= lines.newLine(2,3)

# print "nombre: ",nombre," p1:",p1.nombre," p2:",p2.nombre
lines.defaultTag= 4
l4= lines.newLineSequence()
l4.addLines(xc.ID([1,2,3]))
l4.nDiv= NumDiv

''' 
for_each_lado

  print "nombre: ",edge.nombre," directo:",edge.directo
  edge{print " p1:",p1.nombre," p2:",p2.nombre 

'''


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

nlinPline= l4.getNumEdges
nnodPline= l4.getNumNodes
nelemPline= l4.getNumElements



''' 
print "núm. nodos: ",nnod
nodes= preprocessor.getNodeLoader

for_each
  print "  nodo: ",tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]
               

print "núm. elementos: ",nelem
'''
elementos= setTotal.getElements
ratio1= 0.0
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elementos:
  # print "  elem: ",tag," nod. I: ",nod[0].tag," nod. J: ",nod[1].tag," L= ",length
# print "lteor: ",(lteor)
  ratio1= (e.getL()-lteor)/lteor

ratio2= (nnodPline-(NumDiv+1))
ratio3= (nelemPline-NumDiv)
ratio4= (nlinPline-3)

'''
print "ratio1: ", ratio1
print "ratio2= ", ratio2
print "ratio3= ", ratio3
print "nlinPline= ", nlinPline
print "ratio4= ", ratio4
'''

import os
fname= os.path.basename(__file__)
if (ratio1<1e-10) & (ratio4<1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
