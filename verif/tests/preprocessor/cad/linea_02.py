# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDiv= 1
CooMax= 10

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

# Problem type
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define materials
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
l= lines.newLine(1,2)
l.nDiv= NumDiv

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)

mesh= prueba.getDomain.getMesh
nnod= mesh.getNumNodes()

# nIter= mesh.getNodeIter
# nod= nIter.next()
# while not(nod is None):
#   print "tag= ", nod.tag, " x= ", nod.getCoo[0], " y= ", nod.getCoo[1], " z= ", nod.getCoo[2]
#   nod= nIter.next()

nnodteor= NumDiv+1
ratio1= abs(nnodteor-nnod)/nnodteor
# print("ratio1= ",tonum(ratio1),"\n")

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-12:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


