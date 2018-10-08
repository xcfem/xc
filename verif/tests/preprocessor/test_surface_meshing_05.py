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

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

nNodes= 0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elasticMembranePlateSectionTest= typical_materials.defElasticMembranePlateSection(preprocessor, "elasticMembranePlateSectionTest",E,nu,rho,0.25)

nodes= preprocessor.getNodeHandler
nodes.newSeedNode()

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elasticMembranePlateSectionTest"
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
points.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))
points.newPntFromPos3d(geom.Pos3d(R,0.0,0.5))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.5))
points.newPntFromPos3d(geom.Pos3d(0.0,R,0.5))
points.newPntFromPos3d(geom.Pos3d(R,0.0,1.0))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),1.0))
points.newPntFromPos3d(geom.Pos3d(0.0,R,1.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfaceGridPts([[1,2,3],[4,5,6]])
s1.nDivI= 5
s1.nDivJ= 1

s2= surfaces.newQuadSurfaceGridPts([[4,5,6],[7,8,9]])
s2.nDivI= 5
s2.nDivJ= 1


Z1= preprocessor.getSets.defSet("Z1")
Z1.getSurfaces.append(s1)
Z1.fillDownwards()
Z2= preprocessor.getSets.defSet("Z2")
Z2.getSurfaces.append(s2)
Z2.fillDownwards()

feProblem.setVerbosityLevel(0) #Dont print warning messages about element seed.
Z1.genMesh(xc.meshDir.I)
Z2.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) #Print warnings again 


nNodes= preprocessor.getSets.getSet("total").getNodes.size

ratio1= (nNodes-18)/18

''' 
print "nNod= ",nNodes
print "ratio1= ",ratio1
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1<=5e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
