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

# feProblem.setVerbosityLevel(0)

ndivX= 5
ndivY= 8

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",30e6,0.3,0.0)

nodes= preprocessor.getNodeHandler

nodes.dimSpace= 3 # 3 coordinates for each node.
nodes.numDOFs= 2 # Two degrees of freedom for each node.

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast2d.name
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
quad4n= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))

unifGrids= preprocessor.getMultiBlockTopology.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 10
uGrid.Ly= 10
uGrid.Lz= 1
uGrid.nDivX= ndivX
uGrid.nDivY= ndivY
uGrid.nDivZ= 0

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodesTeor= (ndivX+1)*(ndivY+1)
numElemTeor= ndivX*ndivY

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
