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
prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor
# Define materials
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",30e6,0.3,0.0)

nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics3D(nodes)


nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast3d"
seedElemLoader.dimElem= 3 # Dimension of element space
seedElemLoader.defaultTag= 1 #Tag for the next element.
brick= seedElemLoader.newElement("Brick",xc.ID([0,0,0,0,0,0,0,0]));

unifGrids= preprocessor.getCad.getUniformGrids
uGrid= unifGrids.newUniformGrid()

uGrid.Lx= 10
uGrid.Ly= 10
uGrid.Lz= 10
uGrid.nDivX= ndivX
uGrid.nDivY= ndivY
uGrid.nDivZ= ndivZ


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size

numNodesTeor= (ndivX+1)*(ndivY+1)*(ndivZ+1)
numElemTeor= ndivX*ndivY*ndivZ

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
