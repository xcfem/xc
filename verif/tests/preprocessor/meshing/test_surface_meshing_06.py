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

CooMaxX= 10
CooMaxY= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)



elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast2d"
elem= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt0= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt1= points.newPntFromPos3d(geom.Pos3d(CooMaxX,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(0.0,CooMaxY,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(pt0.tag,pt1.tag,pt2.tag,pt3.tag)
s1.nDivI= 4
s1.nDivJ= 2

s1.genMesh(xc.meshDir.I)

modelSpace.getTotalSet().fillDownwards()

numNodesOnPerimeter= -4
for l in s1.getSides:
    edge= l.getEdge
    numNodesOnPerimeter+= len(edge.nodes)
    # for n in edge.nodes:
    #     print('line= ', edge.name, ' node: ',n.tag)

numNodesOnPerimeterRef= 2*s1.nDivI+2*s1.nDivJ
ratio= abs(numNodesOnPerimeter-numNodesOnPerimeterRef)

'''
print(numNodesOnPerimeter)
print(numNodesOnPerimeterRef)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio<=1e-15) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
