# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
import math
import os
import sys
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
# feProblem.setVerbosityLevel(0)

numNodes= 0
numElem= 0

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()
# Materials definition
matPrb= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E,nu,rho,0.25)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "matPrb"
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(1,0,0))
pt2= points.newPntFromPos3d(geom.Pos3d(1,1,0))
pt3= points.newPntFromPos3d(geom.Pos3d(1,1,1))
pt4= points.newPntFromPos3d(geom.Pos3d(1,0,1))
pt5= points.newPntFromPos3d(geom.Pos3d(0,1,0))
pt6= points.newPntFromPos3d(geom.Pos3d(0,1,1))
pt7= points.newPntFromPos3d(geom.Pos3d(0,0,1))
pt8= points.newPntFromPos3d(geom.Pos3d(0,0,0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 1
s.nDivJ= 1

s= surfaces.newQuadSurfacePts(pt3.tag,pt6.tag,pt7.tag,pt4.tag)
s.nDivI= 1
s.nDivJ= 4

s= surfaces.newQuadSurfacePts(pt5.tag,pt6.tag,pt7.tag,pt8.tag)
s.nDivI= 2
s.nDivJ= 3

s= surfaces.newQuadSurfacePts(pt5.tag,pt8.tag,pt1.tag,pt2.tag)
s.nDivI= 4
s.nDivJ= 4

divsOk= surfaces.conciliaNDivs()


setTotal= preprocessor.getSets.getSet("total")
feProblem.setVerbosityLevel(0) #Dont print warning messages about element seed.
setTotal.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) #Print warnings again 


numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size



areaTotal= 0.0
mesh= feProblem.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  areaTotal+= elem.getArea(True)
  elem= eIter.next()

ratio1= abs(numNodes-40)
ratio2= abs(numElem-32)
ratio3= abs(areaTotal-4)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
