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

NumDiv= 8
CooMax= 10

# Problem type
prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor
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
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMax/2.0,CooMax/2.0,CooMax/2.0))
pt= points.newPntIDPos3d(3,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l1= lines.newLineSequence()
l1.addPoints(xc.ID([1,2,3]))
l1.nDiv= NumDiv

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
nnodPline= l1.getNumNodes
nelemPline= l1.getNumElements

''' 
print "number of nodes: ",nnod
nodes= preprocessor.getNodeLoader

for_each
  print "  node: ",tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]

print "number of elements: ",nelem
'''

elements= setTotal.getElements
ratio1= 0.0
vteor2= (CooMax/NumDiv)**2
lteor= math.sqrt(3*vteor2)
for e in elements:
  #print "  elem: ",tag," nod. I: ",nod[0].tag," nod. J: ",nod[1].tag," L= ",e.getL()
  ratio1+= (e.getL()-lteor)/lteor



ratio2= (nnodPline-(NumDiv+1))
ratio3= (nelemPline-NumDiv)

'''
print "ratio1: ", ratio1
print "ratio2= ", ratio2
print "ratio3= ", ratio3
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<=1e-15) & (ratio3<=1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
