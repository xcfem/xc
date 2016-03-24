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
cos135= math.cos(math.radians(135))
sin135= -cos135
cooCentroElemTeor= xc.Vector([-1.36603,1.36603,0])

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodos)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodos.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]));
truss.area= 10.0
puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,R,0.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos135),(R*sin135),0.0))
puntos.newPntFromPos3d(geom.Pos3d(-R,0.0,0.0))

lineas= preprocessor.getCad.getLines
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

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)

nnodos= l1.getNumNodes

elementos= preprocessor.getElementLoader
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
print "theta1= ",(rad2deg(th1))
print "theta2= ",(rad2deg(th2))
print "xc= ",(xc)
print "yc= ",(yc)
print "zc= ",(zc)
print "xi= ",(xi)
print "yi= ",(yi)
print "zi= ",(zi)
print "radius= ",(r)
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
