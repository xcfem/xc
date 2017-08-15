# -*- coding: utf-8 -*-

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

NumDiv= 25
CooMax= 10

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.dimElem= 3 # Dimension of element space
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]));
truss.area= 10.0

points= preprocessor.getCad.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= NumDiv

length= l1.getLong()
l1.splitAtLambda(0.5)

length1= l1.getLong()
l2= lines.get(2)
length2= l2.getLong()


ratio1= abs(length1-length/2.0)
ratio2= abs(length2-length/2.0)

''' 
print "length: ",length
print "length1: ",length1
print "length2: ",length2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1 < 1e-15) & (ratio2 < 1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


