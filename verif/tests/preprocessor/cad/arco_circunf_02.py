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

NumDiv= 13
R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_elasticidad3D(nodes)
nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast"
seedElemLoader.dimElem= 3
seedElemLoader.defaultTag= 1 #Tag for the next element.
truss= seedElemLoader.newElement("truss",xc.ID([0,0]));
truss.area= 10.0

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))

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

nnodes= l1.getNumNodes
'''
print "núm. nodos: ", nnod
nodes= preprocessor.getNodeLoader

for_each
  print "  nodo: ",tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2]

'''

nnodteor= NumDiv+1
ratio1= (nnodteor/nnodes)

''' 
print "ratio1= ",(ratio1)
print "theta1= ",(math.radians(th1))
print "theta2= ",(math.radians(th2))
print "xC= ",(xC)
print "yC= ",(yC)
print "zC= ",(zC)
print "xi= ",(xi)
print "yi= ",(yi)
print "zi= ",(zi)
print "radius= ",(r)
   '''

import os
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
