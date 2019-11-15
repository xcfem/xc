# -*- coding: utf-8 -*-
from __future__ import division
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

# home made test

NumDivI= 3
NumDivJ= 2
CooMax= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)# Materials definition

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

nodes.newSeedNode()

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast2d"
elem= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))


points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(CooMax,CooMax,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,CooMax,0.0))

#             l2
#  4 +--------------------+ 3
#    |                    |
# l4 |                    | l1
#    |        l3          |
#  1 +--------------------+ 2
#

lines= preprocessor.getMultiBlockTopology.getLines
l3= lines.newLine(pt1.tag,pt2.tag)
l2= lines.newLine(pt4.tag,pt3.tag)
l1= lines.newLine(pt2.tag,pt3.tag)
l4= lines.newLine(pt4.tag,pt1.tag)

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfaceLines(l1.tag,l2.tag,l3.tag,l4.tag)
s.nDivI= NumDivI
s.nDivJ= NumDivJ


s.genMesh(xc.meshDir.I)




nnodCuadr= s.getNumNodes
# print "nnod= ",s.getNumNodes
nelemCuadr= s.getNumElements
# print "nelem= ",s.getNumElements



mesh= feProblem.getDomain.getMesh
nnodDom= mesh.getNumNodes()

''' 
print "number of nodes: ",nnod
nodes= preprocessor.getNodeHandler

for_each
  print "  node: ",tag," x= ",coord[0],", y= ",coord[1]

'''
nelemDom= mesh.getNumElements()
#print "number of elements: ",nelem
perim= 0.0

eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
#  print elem.tag," nod. I: ",elem.getNodes[0].tag," nod. J: ",elem.getNodes[1].tag," nod. K: ",elem.getNodes[2].tag," nod. L: ",elem.getNodes[3].tag," L= ",elem.getPerimeter()
  perim+= elem.getPerimeter(True)
  elem= eIter.next()

pMed= perim/nelemCuadr
pMedTeor= 2*CooMax/NumDivI+2*CooMax/NumDivJ




ratio1= (abs(nnodCuadr-(NumDivI+1)*(NumDivJ+1)))
ratio2= (abs(nelemCuadr-(NumDivI*NumDivJ)))
ratio3= (abs(nnodDom-(NumDivI+1)*(NumDivJ+1)))
ratio4= (abs(nelemDom-(NumDivI*NumDivJ)))
ratio5= (abs(pMed-pMedTeor))

''' 
print "perim medio: ",pMed
print "theoretical perim medio: ",pMedTeor
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
 '''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
