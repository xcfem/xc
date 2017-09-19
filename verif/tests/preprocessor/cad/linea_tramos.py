# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 10
CooMax= 10
raiz3= math.sqrt(3)

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
truss= seedElemLoader.newElement("Truss",xc.ID([0,0]))
truss.area= 10

points= preprocessor.getCad.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l= lines.newDividedLine(1,2)
l.setLongs(xc.Vector([raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3,raiz3]))

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

''' 
print "number of nodes: ",nnod
nodes= preprocessor.getNodeLoader
for_each
  print "  node: ",tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]

print "number of elements: ",nelem
'''
elements= setTotal.getElements
cumple= 1
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elements:
  # print "  elem: ",tag," nod. I: ",nod[0].tag," nod. J: ",nod[1].tag," L= ",length
  # print "lteor: ",(lteor)
  ratio1= (lteor/e.getL())
  cumple= (abs(ratio1-1.0)<1e-5) & (cumple) 
  # print "cumple: ",(cumple)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
