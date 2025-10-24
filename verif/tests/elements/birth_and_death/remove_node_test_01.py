# -*- coding: utf-8 -*-
''' Trivial test cheking node removal.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
l= 10.0 
nodeTags= list()
for i in range(0,4):
    newNode= nodes.newNodeXY(i*l,0)
    nodeTags.append(newNode.tag)

numberOfNodesBefore= modelSpace.getNumberOfNodes()

nodesToRemove= [1, 3]
for i in nodesToRemove:
    nodeTag= nodeTags[i]
    node= modelSpace.getNode(nodeTag)
    modelSpace.removeNode(node)
for i in nodesToRemove:
    nodeTags.remove(i)

xi= list()
for nodeTag in nodeTags:
    node= modelSpace.getNode(nodeTag)
    xi.append(node.getInitialPos3d.x)

numberOfNodesAfter= modelSpace.getNumberOfNodes()
numberOfDeletedNodes= numberOfNodesBefore-numberOfNodesAfter
testOK= (numberOfDeletedNodes==2)
testOK= (math.sqrt(xi[0]**2+(xi[1]-2*l)**2)<1e-15)

'''                    
print(numberOfNodesBefore)
print(numberOfNodesAfter)
print(numberOfDeletedNodes)
print(nodeTags)
print(xi)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

