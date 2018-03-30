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

ndivZ= 500

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast"
seedElemHandler.dimElem= 3 # Dimension of element space
seedElemHandler.defaultTag= 1 #Tag for the next element.
truss= seedElemHandler.newElement("Truss",xc.ID([0,0]));
truss.area= 10.0

unifGrids= preprocessor.getMultiBlockTopology.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 1
uGrid.Ly= 1
uGrid.Lz= 10
uGrid.nDivX= 0
uGrid.nDivY= 0
uGrid.nDivZ= ndivZ

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodesTeor= ndivZ+1
numElemTeor= ndivZ

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
