# -*- coding: utf-8 -*-
from __future__ import print_function
# getNearestNode method test
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

tg= 0
lapso= 0
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
for i in range(1,1000):
  n= nodes.newNodeXYZ(i,0,0)

# print("created ",nnod," nodes.\n")
mesh= feProblem.getDomain.getMesh
# crono= 1
# \crono{\start{}}
n= mesh.getNearestNode(geom.Pos3d(50.51,0,0))
tg= n.tag
#\crono{\stop{} lapso= userTime}




''' 
print("tag node: ",tg)
print("lapso: ",lapso)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((tg==50) & (lapso<100)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

