# -*- coding: utf-8 -*-
# Test para comprobar el funcionamiento de getNearestElement
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

numElementos= 1000
tg= 0
lapso= 0
i= 1
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
nodes.defaultTag= 1
for i in range(1,1001):
  n= nodes.newNodeXYZ(i,0,0)

# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",2.1e6)

elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "elast"
elementos.dimElem= 2 # Dimension of element space
elementos.defaultTag= 1 #Tag for the next element.
for i in range(1,numElementos):
  truss= elementos.newElement("truss",xc.ID([i,i+1]));
  truss.area= 1

mesh= prueba.getDomain.getMesh
# print "creados ",nnod," nodes.\n"
start_time= time.time()
tg= mesh.getNearestElement(geom.Pos3d(50.51,0,0)).tag
lapso= time.time()-start_time


''' 
print "tag elemento: ",tg
print "lapso: ",lapso
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((tg==50)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


