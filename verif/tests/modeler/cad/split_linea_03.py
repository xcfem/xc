# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

NumDiv= 25
CooMax= 10

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)
# Definimos materiales
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

lineas= mdlr.getCad.getLines
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

