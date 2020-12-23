# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

K= 1000 # Spring constant
l= 100 # Distance between nodes
F= 1 # Force magnitude

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)

xzVector= modelSpace.getSuitableXZVector(n1,n2)
ratio1= (xzVector-xc.Vector([0,0,1])).Norm()

'''
print('xzVector= ',xzVector)
print('xzVector= ',ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')


