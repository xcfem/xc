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


NumDiv= 2
R= 2.0
cos225= math.cos(math.radians(22.5))
sin225= math.sin(math.radians(22.5))
cos45= math.cos(math.radians(45))
sin45= cos45
cos675= math.cos(math.radians(67.5))
sin675= math.sin(math.radians(67.5))

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)



seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast"
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
truss= seedElemHandler.newElement("Truss",xc.ID([0,0]))
truss.sectionArea= 10.0
points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(R*cos225,R*sin225,0.0))
pt= points.newPntIDPos3d(3,geom.Pos3d((R*cos45),(R*sin45),0.0))
pt= points.newPntIDPos3d(4,geom.Pos3d((R*cos675),(R*sin675),0.0))
pt= points.newPntIDPos3d(5,geom.Pos3d(0.0,R,0.0))

lines= preprocessor.getMultiBlockTopology.getLines
lines.defaultTag= 1
l1= lines.newCircleArc(1,2,3)
l1.nDiv= NumDiv
th1= l1.getTheta1()
th2= l1.getTheta2()
length= l1.getLength()
xC= l1.getCenter().x
yC= l1.getCenter().y
zC= l1.getCenter().z
xi= l1.getPInic().x
yi= l1.getPInic().y
zi= l1.getPInic().z
r= l1.getRadius()

l2= lines.newCircleArc(3,4,5)
l2.nDiv= NumDiv


l1= preprocessor.getSets.getSet("l1")
l1.genMesh(xc.meshDir.I)

l2= preprocessor.getSets.getSet("l2")
l2.genMesh(xc.meshDir.I)


nnodes= l1.getNumNodes+l2.getNumNodes-1
nodes= preprocessor.getNodeHandler
nod2= nodes.getNode(2)
x2= nod2.get3dCoo[0]
y2= nod2.get3dCoo[1]
nod4= nodes.getNode(4)
x4= nod4.get3dCoo[0]
y4= nod4.get3dCoo[1]


nnodteor= 2*NumDiv+1
ratio1= (nnodteor/nnodes)
ratio2= (y4-x2)**2+(y2-x4)**2

''' 
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
print "ratio2= ",(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-12) & (abs(ratio2)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
