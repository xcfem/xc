# -*- coding: utf-8 -*-
''' Test sets substract method with elements.'''

import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Create nodes.
n0= modelSpace.newNodeXY(0,0)
n1= modelSpace.newNodeXY(1,0)
n2= modelSpace.newNodeXY(2,0)
n3= modelSpace.newNodeXY(3,1)
n4= modelSpace.newNodeXY(4,1)

# Define materials.
dummyMaterial= typical_materials.defElasticMaterial(preprocessor, "dummyMaterial",E=2.1e9, nu= 0.3)

# Create elements.
modelSpace.setDefaultMaterial(dummyMaterial)
modelSpace.setElementDimension(2) # Dimension of element space
trussA= modelSpace.newElement("Truss", [n0.tag, n1.tag])
trussA.area= 0.1
trussB= modelSpace.newElement("Truss", [n1.tag, n2.tag])
trussB.area= 0.1
trussC= modelSpace.newElement("Truss", [n2.tag, n3.tag])
trussC.area= 0.1
trussD= modelSpace.newElement("Truss", [n3.tag, n4.tag])
trussD.area= 0.1

# Create sets.
s1= modelSpace.defSet('s1', elements= [trussA, trussB, trussC, trussD])
s1.fillDownwards()
s2= modelSpace.defSet('s2', elements= [trussB, trussC])
s2.fillDownwards()

# Check results.
numNodesBefore= len(s1.nodes)
numElementsBefore= len(s1.elements)
s1.substract(s2.elements)
s1.substract(s2.nodes)
s1.fillDownwards()
numNodesAfter= len(s1.nodes)
tagsNodesAfter= s1.getNodeTagList()
numElementsAfter= len(s1.elements)
tagsElementsAfter= s1.getElementTagList()

testOK= (((numNodesBefore-numNodesAfter)-1)==0)
for n in [n0, n1, n3, n4]:
    testOK and (n.tag in tagsNodesAfter)
testOK= testOK and (((numElementsBefore-numElementsAfter)-len(s2.elements))==0)
for e in [trussA, trussD]:
    testOK and (e.tag in tagsElementsAfter)

'''
print('numNodesBefore= ', numNodesBefore)
print('numNodesAfter= ', numNodesAfter)
print('tagsNodesAfter= ', tagsNodesAfter)
print('numElementsBefore= ', numElementsBefore)
print('numElementsAfter= ', numElementsAfter)
print('tagsElementsAfter= ', tagsElementsAfter)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
