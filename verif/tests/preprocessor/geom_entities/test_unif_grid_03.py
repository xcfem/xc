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

ndivX= 5
ndivY= 8
ndivZ= 2

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

unifGrids= preprocessor.getMultiBlockTopology.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 10
uGrid.Ly= 10
uGrid.Lz= 10
uGrid.nDivX= ndivX
uGrid.nDivY= ndivY
uGrid.nDivZ= ndivZ

# Define materials
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",30e6,0.3,0.0)

nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Mesh generation

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast3d"
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
brick= seedElemHandler.newElement("Brick",xc.ID([0,0,0,0,0,0,0,0]))

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodesTeor= (ndivX+1)*(ndivY+1)*(ndivZ+1)
numElemTeor= ndivX*ndivY*ndivZ

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
