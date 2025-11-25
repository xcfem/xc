# -*- coding: utf-8 -*-
''' Test sets substract methods with nodes.'''

import xc
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Create nodes.
n0= modelSpace.newNodeXY(0,0)
n1= modelSpace.newNodeXY(1,0)
n2= modelSpace.newNodeXY(2,0)
n3= modelSpace.newNodeXY(3,1)

# Create sets.
s1= modelSpace.defSet('s1', nodes= [n0, n1, n2, n3])
s2= modelSpace.defSet('s2', nodes= [n1, n3])

# Check results.
numNodesBefore= len(s1.nodes)
s1.substract(s2.nodes)
numNodesAfter= len(s1.nodes)
testOK= (((numNodesBefore-numNodesAfter)-len(s2.nodes))==0)
tagsNodesAfter= s1.getNodeTagList()
testOK= testOK and (n0.tag in tagsNodesAfter) and (n2.tag in tagsNodesAfter)

'''
print('numNodesBefore= ', numNodesBefore)
print('numNodesAfter= ', numNodesAfter)
print('tagsNodesAfter= ', tagsNodesAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
