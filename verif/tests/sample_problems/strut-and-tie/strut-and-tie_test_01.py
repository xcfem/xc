# -*- coding: utf-8 -*-
''' Naive approach to calculation of pile cap on two piles. Home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from misc_utils import log_messages as lmsg

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Problem geometry.
# 
#        n0
#        +
#       / \
#      /   \    d
#     /     \
#   +--------+
# n1    S    n2
#

s= 1.6
d= 1.0
v= s/2.0

## Define mesh.
n0= modelSpace.newNode(0.0, d)
n1= modelSpace.newNode(-v, 0.0)
n2= modelSpace.newNode(v, 0.0)

## Define materials.
reinfSteel= EC2_materials.S500B
tieArea= 5.8e-4
concrete= EC2_materials.C30
strutArea= 0.25

## Define elements.
### Define tie.
modelSpace.setDefaultMaterial(reinfSteel.defElasticMaterial(preprocessor= preprocessor))
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
tie= modelSpace.newElement("Truss", [n1.tag, n2.tag])
tie.sectionArea= tieArea
### Define struts.
modelSpace.setDefaultMaterial(concrete.defElasticMaterial(preprocessor= preprocessor))
strut1= modelSpace.newElement("Truss", [n1.tag, n0.tag])
strut1.sectionArea= strutArea
strut2= modelSpace.newElement("Truss", [n2.tag, n0.tag])
strut2.sectionArea= strutArea

## Define constraints
modelSpace.fixNode('00', n1.tag)
modelSpace.fixNode('F0', n2.tag)

# Load definition.
F= 1.5*183e3
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n0.tag,xc.Vector([0,-F]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= False)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

# Check results.
T= tie.getN()
Tref= F*v/2.0/d
ratio1= abs(T-Tref)/Tref

'''
print('T= ', T/1e3, 'kN')
print('Tref= ', Tref/1e3, 'kN')
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayIntForcDiag('N')
# # oh.displayDispRot(itemToDisp='uY')

