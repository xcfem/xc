# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

NumDiv= 3
R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45
cooCentroElemTeor= xc.Vector([1.36603,1.36603,0])

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)

# Materials definition
elast= typical_materials.defElasticMaterial(mdlr,"elast",3000)

nodos.newSeedNode()
seedElemLoader= mdlr.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]));
truss.area= 10.0

puntos= mdlr.getCad.getPoints
pt1= puntos.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
pt2= puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
pt3= puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))

lineas= mdlr.getCad.getLines
lineas.defaultTag= 1
l= lineas.newCircleArc(1,2,3)
l.nDiv= NumDiv
th1= l.getTheta1()
th2= l.getTheta2()
long= l.getLong()
xC= l.getCentro().x
yC= l.getCentro().y
zC= l.getCentro().z
xi= l.getPInic().x
yi= l.getPInic().y
zi= l.getPInic().z
r= l.getRadio()

l1= mdlr.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)

nnodos= l1.getNumNodes

elementos= mdlr.getElementLoader
ele2= elementos.getElement(2)
puntos= ele2.getCooPuntos(2) #Two divisions-> Three points.
cooCentroElem= puntos.getRow(1)



nnodteor= NumDiv+1
ratio1= (nnodteor/nnodos)
ratio2= (cooCentroElem-cooCentroElemTeor).Norm()

''' 
print "nnodos= ",(nnodos)
print "nnodteor= ",(nnodteor)
print "ratio1= ",(ratio1)
print "theta1= ",(math.radians(th1))
print "theta2= ",(math.radians(th2))
print "xC= ",(xC)
print "yC= ",(yC)
print "zC= ",(zC)
print "xi= ",(xi)
print "yi= ",(yi)
print "zi= ",(zi)
print "radio= ",(r)
print "puntos= ",puntos
print "cooCentroElem= ",cooCentroElem
print "cooCentroElemTeor= ",cooCentroElemTeor
print "ratio2= ",(ratio2)
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-12) & (abs(ratio2)<1e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
