# -*- coding: utf-8 -*-
'''8 node brick element Patch test.

Reference: MacNeal & Harder, “A Proposed Standard Set of Problems to Test Finite Element Accuracy”, Finite Elements in Analysis and Design, 1 (1985) 3-20
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from postprocess import output_handler
import numpy as np
from numpy import linalg as LA
from misc import matrix_utils

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast3d= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod0= nodes.newNodeXYZ(0,0,0)
nod1= nodes.newNodeXYZ(1,0,0)
nod2= nodes.newNodeXYZ(1,1,0)
nod3= nodes.newNodeXYZ(0,1,0)
nod4= nodes.newNodeXYZ(0,0,1)
nod5= nodes.newNodeXYZ(1,0,1)
nod6= nodes.newNodeXYZ(1,1,1)
nod7= nodes.newNodeXYZ(0,1,1)

elements= preprocessor.getElementHandler
elements.defaultMaterial= elast3d.name


brick= elements.newElement("Brick",xc.ID([nod0.tag, nod1.tag, nod2.tag, nod3.tag, nod4.tag, nod5.tag, nod6.tag, nod7.tag]))

naturalCoord= [[-1,-1,-1], [1,-1,-1], [1,1,-1], [-1,1,-1],
               [-1,-1,+1], [1,-1,+1], [1,1,+1], [-1,1,+1]]

testValues= np.array([[1, 0, 0, 0, 0, 0, 0, 0],
             [0, 1, 0, 0, 0, 0, 0, 0], 
             [0, 0, 1, 0, 0, 0, 0, 0], 
             [0, 0, 0, 1, 0, 0, 0, 0], 
             [0, 0, 0, 0, 1, 0, 0, 0], 
             [0, 0, 0, 0, 0, 1, 0, 0], 
             [0, 0, 0, 0, 0, 0, 1, 0], 
             [0, 0, 0, 0, 0, 0, 0, 1]])

resultNodes= list()
for nc in naturalCoord:
    values= brick.getShapeFunctionsValues(nc[0],nc[1],nc[2])
    row= list()
    for v in values:
        row.append(v)
    resultNodes.append(row)

err= LA.norm(np.array(resultNodes)-testValues)

# Verifies that Gauss Point i correspond to Node i
gaussPointsPositions= matrix_utils.matrixToNumpyArray(brick.gaussPointsPositions)
gaussPointsPositions*= math.sqrt(3.0)
err+= LA.norm(np.array(gaussPointsPositions)-naturalCoord)


    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-20):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
