# -*- coding: utf-8 -*-
# Test para comprobar el funcionamiento de getNearestNode
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

tg= 0
lapso= 0
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
for i in range(1,1000):
  n= nodes.newNodeXYZ(i,0,0)

# print "creados ",nnod," nodes.\n"
mesh= prueba.getDomain.getMesh
# crono= 1
# \crono{\start{}}
n= mesh.getNearestNode(geom.Pos3d(50.51,0,0))
tg= n.tag
#\crono{\stop{} lapso= userTime}




''' 
print "tag nodo: ",tg
print "lapso: ",lapso
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((tg==50) & (lapso<100)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

