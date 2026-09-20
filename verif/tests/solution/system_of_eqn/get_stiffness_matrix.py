# -*- coding: utf-8 -*-
''' Checks the getA method.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions


# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodeHandler)

## Define mesh.
### Define nodes.
l= 10 #Bar length
n1= modelSpace.newNode(0)
n2= modelSpace.newNode(l)

### Define material.
E= 30e6 #Young modulus
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

### Define elements.
''' We define nodes at the points where loads will be applied.
    We will not compute stresses, so we can use an arbitrary
    cross section of unit area.'''
a= 0.01
modelSpace.setDefaultMaterial(elast) # Set the material for the new element.
modelSpace.setElementDimension(1) # Truss defined in a two-dimensional space.
truss= modelSpace.newElement("Truss",nodeTags= [n1.tag,n2.tag])
truss.sectionArea= a

### Define constraints.
modelSpace.fixNode('0', n1.tag)

### Define loads.
lp0= modelSpace.newLoadPattern(name= '0', setCurrent= True)
F= 1e3
lp0.newNodalLoad(n1.tag,xc.Vector([F]))
# Add the load pattern to the domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
solProc= predefined_solutions.FullGenStaticLinear(feProblem)
result= solProc.solve()

# Check results.
## Stiffness matrix.
K= solProc.getA()
kRef= E*a/l
ratio1= abs(K[0][0]-kRef)/kRef
## Reaction.
modelSpace.calculateNodalReactions()
R= n1.getReaction[0]
ratio2= abs(R+F)/F

'''
print(K)
print(ratio1)
print(R)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio2)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayDispRot(itemToDisp='uY')


