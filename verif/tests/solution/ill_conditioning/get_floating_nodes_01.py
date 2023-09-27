# -*- coding: utf-8 -*-
''' Check getFloatingNodes method.'''

from __future__ import print_function
from __future__ import division
import xc

from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Mesh generation.

### Nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(1,1)
n3= nodes.newNodeXY(2,2) # Floating node.

setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

### Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1,gdl 0 translation X
spc= constraints.newSPConstraint(n1.tag,1,0.0) # Node 1,gdl 1 translation Y
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2,gdl 0 translation X
spc= constraints.newSPConstraint(n2.tag,1,0.0) # Node 2,gdl 1 translation Y

# Search for floating nodes.
floatingNodes= modelSpace.getFloatingNodes()

# Check result.
floatingNodeTag= floatingNodes[0].tag
ok= (floatingNodeTag==n3.tag)

''' 
modelSpace.checkFloatingNodes()
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ok):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
