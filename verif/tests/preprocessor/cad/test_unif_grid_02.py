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

# nverborrea= 0

ndivX= 5
ndivY= 8

prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",30e6,0.3,0.0)

nodes= preprocessor.getNodeLoader

nodes.dimSpace= 3 # 3 coordinates for each node.
nodes.numGdls= 2 # Two degrees of freedom for each node.
nodes.newSeedNode()
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast2d"
seedElemLoader.dimElem= 3 # Dimension of element space
seedElemLoader.defaultTag= 1 #Tag for the next element.
quad4n= seedElemLoader.newElement("FourNodeQuad",xc.ID([0,0,0,0]));

unifGrids= preprocessor.getCad.getUniformGrids
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(numNodesTeor-numNodes)<1e-15) & (abs(numElemTeor-numElem)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
