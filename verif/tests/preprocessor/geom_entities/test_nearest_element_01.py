# -*- coding: utf-8 -*-
# getNearestElement method test.
import xc_base
import geom
import xc
from solution import predefined_solutions
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
nodes.defaultTag= 1
for i in range(1,1001):
  n= nodes.newNodeXYZ(i,0,0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",2.1e6)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for the next element.
for i in range(1,numElements):
  truss= elements.newElement("Truss",xc.ID([i,i+1]))
  truss.sectionArea= 1

mesh= feProblem.getDomain.getMesh
# print "creados ",nnod," nodes.\n"
start_time= time.time()
tg= mesh.getNearestElement(geom.Pos3d(50.51,0,0)).tag
lapso= time.time()-start_time


''' 
print "element tag: ",tg
print "lapse: ",lapse
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((tg==50)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


