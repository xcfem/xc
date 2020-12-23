# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc

from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,1)

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1,gdl 0 translation X
spc= constraints.newSPConstraint(n1.tag,1,0.0) # Node 1,gdl 1 translation Y
spc= constraints.newSPConstraint(n1.tag,2,0.0) # Node 1,gdl 2 rotation.
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 1,gdl 0 translation X
spc= constraints.newSPConstraint(n2.tag,1,0.0) # Node 1,gdl 1 translation Y
#spc= constraints.newSPConstraint(n2.tag,2,0.0) # Node 1,gdl 2 rotation.


analOk= modelSpace.zeroEnergyModes(1)
eig1= modelSpace.analysis.getEigenvalue(1)
v1= n1.getEigenvector(1)
v2= n2.getEigenvector(1)

ratio1= eig1
ratio2= v2[0]**2+v2[1]**2+(v2[2]-1)**2

''' 
print("eig1= ",(eig1))
print("v1= ",(v1))
print("n1.getEigenvector(1)= ",v1)
print("n2.getEigenvector(1)= ",v2)
print("ratio1= ", ratio1)
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<0.06):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
