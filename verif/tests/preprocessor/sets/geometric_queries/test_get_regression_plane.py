# -*- coding: utf-8 -*-
from __future__ import print_function
''' Home made test.
    Obtaining the contour of an element set. '''

import xc
import geom
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't print log messages
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Create some nodes in the XZ plane:
n1= nodes.newNodeXYZ(1,0,0)
n2= nodes.newNodeXYZ(2,0,1)
n3= nodes.newNodeXYZ(3,0,1)
n4= nodes.newNodeXYZ(4,0,0)
n5= nodes.newNodeXYZ(5,0,2)
n6= nodes.newNodeXYZ(6,0,2)
n7= nodes.newNodeXYZ(7,0,2)
n8= nodes.newNodeXYZ(8,0,0)

# Compute the regression plane of the node positions.
xcTotalSet= modelSpace.getTotalSet()
regressionPlane= xcTotalSet.nodes.getRegressionPlane(0.0) # 0.0 -> scale factor for the current position: 1.0 (curreentPosition= initialPosition+scaleFactor*nodeDisplacement).

# Compute normal vector.
normalVector= regressionPlane.getNormal()

# Check the result.
err= (normalVector-geom.Vector3d(0,1,0)).getModulus()
if(err>0): # maybe it's the opposite.
    err= (normalVector-geom.Vector3d(0,-1,0)).getModulus()

'''
print(regressionPlane)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
