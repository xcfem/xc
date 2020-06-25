# -*- coding: utf-8 -*-
from __future__ import print_function

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

NumDiv= 3
R= 2.0
cos135= math.cos(math.radians(135))
sin135= -cos135
cooCentroElemTeor= xc.Vector([-1.36603,1.36603,0])

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)


seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast"
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
truss= seedElemHandler.newElement("Truss",xc.ID([0,0]))
truss.sectionArea= 10.0
points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,R,0.0))
points.newPntFromPos3d(geom.Pos3d((R*cos135),(R*sin135),0.0))
points.newPntFromPos3d(geom.Pos3d(-R,0.0,0.0))

lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l= lines.newCircleArc(1,2,3)
l.nDiv= NumDiv
th1= l.getTheta1()
th2= l.getTheta2()
length= l.getLength()
xC= l.getCenter().x
yC= l.getCenter().y
zC= l.getCenter().z
xi= l.getPInic().x
yi= l.getPInic().y
zi= l.getPInic().z
r= l.getRadius()

l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)

nnodes= l1.getNumNodes

elements= preprocessor.getElementHandler
ele2= elements.getElement(2)
points= ele2.getCooPoints(2) #Two divisions-> Three points.
cooCentroElem= points.getRow(1)

nnodteor= NumDiv+1
ratio1= (nnodteor/nnodes)
ratio2= (cooCentroElem-cooCentroElemTeor).Norm()


''' 
print("nnodes= ",(nnodes))
print("nnodteor= ",(nnodteor))
print("ratio1= ",(ratio1))
print("theta1= ",(rad2deg(th1)))
print("theta2= ",(rad2deg(th2)))
print("xc= ",(xc))
print("yc= ",(yc))
print("zc= ",(zc))
print("xi= ",(xi))
print("yi= ",(yi))
print("zi= ",(zi))
print("radius= ",(r))
print("cooCentroElem= ",cooCentroElem)
print("cooCentroElemTeor= ",cooCentroElemTeor)
print("ratio2= ",(ratio2))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-12) & (abs(ratio2)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
