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

CooMaxX= 3
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
#feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages

numNodes= 0
numElem= 0

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nodes.newSeedNode()

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)


seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast2d"
elem= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))


points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMaxX/3.0,0.0,0.0))
pt= points.newPntIDPos3d(3,geom.Pos3d(CooMaxX*2/3.0,0.0,0.0))
pt= points.newPntIDPos3d(4,geom.Pos3d(CooMaxX,0.0,0.0))
pt= points.newPntIDPos3d(5,geom.Pos3d(0.0,CooMaxY,0.0))
pt= points.newPntIDPos3d(6,geom.Pos3d(CooMaxX/3.0,CooMaxY,0.0))
pt= points.newPntIDPos3d(7,geom.Pos3d(CooMaxX*2/3.0,CooMaxY,0.0))
pt= points.newPntIDPos3d(8,geom.Pos3d(CooMaxX,CooMaxY,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfacePts(1,2,6,5)
s1.nDivI= 1
s1.nDivJ= 1

# print "s1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ

s2= surfaces.newQuadSurfacePts(2,3,7,6)
s2.nDivI= 2
s2.nDivJ= 1

divsOk= surfaces.conciliaNDivs()

# print "\ns1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ
# print "s2 nDivI= ", s2.nDivI
# print "s2 nDivJ= ", s2.nDivJ

s3= surfaces.newQuadSurfacePts(3,4,8,7)
s3.nDivI= 5
s3.nDivJ= 5

divsOk= divsOk & surfaces.conciliaNDivs()


# print "\ns1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ
# print "s2 nDivI= ", s2.nDivI
# print "s2 nDivJ= ", s2.nDivJ
# print "s3 nDivI= ", s3.nDivI
# print "s3 nDivJ= ", s3.nDivJ

s2.nverborrea= 5
total= preprocessor.getSets.getSet("total")
feProblem.setVerbosityLevel(0) #Dont print warning messages about element seed.
total.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) #Print warnings again 

numNodes= total.getNodes.size
numElem= total.getElements.size



ratio1= abs(numNodes-54)
ratio2= abs(numElem-40)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) &divsOk:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
