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
predefined_spaces.gdls_elasticidad3D(nodes)
# Define materials
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
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))

lineas= preprocessor.getCad.getLines
lineas.defaultTag= 1
l1= lineas.newLine(1,2)
l1.nDiv= NumDiv

longitud= l1.getLong()
l1.splitAtNaturalCoordinate(0.0)
l2= lineas.get(2) 
longitud1= l1.getLong()
l2= lineas.get(2)
longitud2= l2.getLong()


ratio1= abs(longitud1-longitud/2)
ratio2= abs(longitud2-longitud/2)

''' 
print "longitud: ",longitud
print "longitud1: ",longitud1
print "longitud2: ",longitud2
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
fname= os.path.basename(__file__)
if((ratio1 < 1e-15) & (ratio2 < 1e-15)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

