# -*- coding: utf-8 -*-
from __future__ import print_function
# getNearestElement method test.
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import time

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

numElements= 1000
tg= 0
lapso= 0
i= 1
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
nodeList= list()
for i in range(0,numElements):
  n= nodes.newNodeXYZ(i,0,0)
  nodeList.append(n)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",2.1e6)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast.name
elements.dimElem= 2 # Dimension of element space
for i in range(1,numElements):
  nA= nodeList[i-1]
  nB= nodeList[i]
  truss= elements.newElement("Truss",xc.ID([nA.tag,nB.tag]))
  truss.sectionArea= 1

mesh= feProblem.getDomain.getMesh
# print("creados ",nnod," nodes.\n")
start_time= time.time()
tg= mesh.getNearestElement(geom.Pos3d(50.51,0,0)).tag
lapse= time.time()-start_time


''' 
print("element tag: ",tg)
print("lapse: ",lapse)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((tg==50)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


