# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

from __future__ import division
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(5,5)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
# Materials
section= typical_materials.defElasticSection2d(preprocessor, "section",1,1,1)
    

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin" # Coordinate transformation for the new elements
elements.defaultMaterial= "section"
elements.defaultTag= 1 #Tag for next element.
beam2d= elements.newElement("ElasticBeam2d",xc.ID([1,2]))

setTotal= preprocessor.getSets.getSet("total")
elems= setTotal.getElements
for e in elems:
  crdTransf= e.getCoordTransf
  #print("vector I:",crdTransf.getIVector)
  vILocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getIVector)
  #print("vector I en locales:",vILocal)
  #print("vector J:",crdTransf.getJVector)
  vJLocal= crdTransf.getVectorLocalCoordFromGlobal(crdTransf.getJVector)
  #print("vector J en locales:",vJLocal)
  dif1= vILocal-xc.Vector([1,0])
  ratio1= dif1.Norm()
  ratio2= (vJLocal-xc.Vector([0,1])).Norm()

# print("vILocal= ", vILocal)
# print("vJLocal= ", vJLocal)
# print("ratio1= ", ratio1)
# print("ratio2= ", ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
