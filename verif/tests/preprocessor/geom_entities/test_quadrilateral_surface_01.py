# Home made test
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

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

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

feProblem.logFileName= "/tmp/erase.log" # Nicely avoid warning messages.
NumDivI= 3
NumDivJ= 2
CooMax= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density




elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast2d.name
quad4n= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))

#               
#  4 +--------------------+ 3
#    |                    |
#    |                    |
#    |                    |
#  1 +--------------------+ 2
#

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(CooMax,CooMax,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,CooMax,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= NumDivI
s.nDivJ= NumDivJ

testEqualOperator= (s==s)

s.genMesh(xc.meshDir.I)

nnodCuadr= s.getNumNodes
# print("nnod= ",s.getNumNodes)
nelemCuadr= s.getNumElements
# print("nelem= ",s.getNumElements)

mesh= feProblem.getDomain.getMesh
nnodDom= mesh.getNumNodes()

# print("number of nodes: ",nnod)
# nIter= mesh.getNodeIter
# nod= nIter.next()
# while not(nod is None):
#   print("tag= ", nod.tag, " x= ", nod.getCoo[0], " y= ", nod.getCoo[1])
#   nod= nIter.next()

nelemDom= mesh.getNumElements()

# print("number of elements: ", mesh.getNumElem())
perim= 0.0

eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
#  print(elem.tag," nod. I: ",elem.getNodes[0].tag," nod. J: ",elem.getNodes[1].tag," nod. K: ",elem.getNodes[2].tag," nod. L: ",elem.getNodes[3].tag," L= ",elem.getPerimeter())
  perim+= elem.getPerimeter(True)
  elem= eIter.next()

pMed= perim/nelemCuadr
pMedTeor= 2*CooMax/NumDivI+2*CooMax/NumDivJ

ratio1= abs(nnodCuadr-(NumDivI+1)*(NumDivJ+1))
ratio2= abs(nelemCuadr-(NumDivI*NumDivJ))
ratio3= abs(nnodDom-(NumDivI+1)*(NumDivJ+1))
ratio4= abs(nelemDom-(NumDivI*NumDivJ))
ratio5= abs(pMed-pMedTeor)

# print("perim medio: ",pMed)
# print("theoretical perim medio: ",pMedTeor)
# print("ratio1= ",ratio1)
# print("ratio2= ",ratio2)
# print("ratio3= ",ratio3)
# print("ratio4= ",ratio4)
# print("ratio5= ",ratio5)
# print(testEqualOperator)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10) & testEqualOperator):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

