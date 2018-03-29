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

CooMaxX= 2
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
old_stderr= sys.stderr

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages
feProblem.errFileName= "/tmp/borrar.err" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()

elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast2d"
elem= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMaxX/2,0.0,0.0))
pt= points.newPntIDPos3d(3,geom.Pos3d(CooMaxX,0.0,0.0))
pt= points.newPntIDPos3d(4,geom.Pos3d(0.0,CooMaxY,0.0))
pt= points.newPntIDPos3d(5,geom.Pos3d(CooMaxX/2,CooMaxY,0.0))
pt= points.newPntIDPos3d(6,geom.Pos3d(CooMaxX,CooMaxY,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfacePts(1,2,5,4)
s1.nDivI= 1
s1.nDivJ= 1



#f1= preprocessor.getSets.getSet("f1")
s1.genMesh(xc.meshDir.I)



surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 2
s2= surfaces.newQuadSurfacePts(2,3,6,5)
s2.nDivI= 2
s2.nDivJ= 2



f2= preprocessor.getSets.getSet("f2")
s2.genMesh(xc.meshDir.I)

total= preprocessor.getSets.getSet("total")
numNodes= total.getNodes.size
numElem= total.getElements.size



''' 
print "Number of nodes: ",numNodes
print "Number of elements: ",numElem
   '''

ratio1= abs(numNodes-8)
ratio2= abs(numElem-3)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
