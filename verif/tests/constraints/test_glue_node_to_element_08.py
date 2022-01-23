# -*- coding: utf-8 -*-
''' Home made test. Check newGlueNodeToElement function.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

A= 31e-4 # Beam cross-section area.
E= 2.0e11 # Young modulus.
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))
I= 2810e-8 # Inertia of the beam section.
sry= 2.42 # Shear coefficient.
Ay= A/sry
h= math.sqrt(A) # Beam cross-section depth.
l= 10 # Bar length in inches
F= 1000.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0)
nA= nodes.newNodeXY(l/2.0,0) # node to be glued.

# Materials definition
scc= typical_materials.defElasticShearSection2d(preprocessor, "scc",A,E,G,I,alpha= Ay/A)

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Element definition
elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bars defined ina a two-dimensional space.
elements.defaultMaterial= scc.name
elements.defaultTransformation= lin.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beam2d.h= h

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode00F(n1.tag)
modelSpace.fixNode00F(n2.tag)

## Glued node.
gluedDOFs= [0,1]
loadOnDOFs= [0,-F,0]
for i in range(0,3):
  if i not in gluedDOFs:
    modelSpace.constraints.newSPConstraint(nA.tag,i,0.0)

glue= modelSpace.constraints.newGlueNodeToElement(nA,beam2d,xc.ID(gluedDOFs))

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nA.tag,xc.Vector(loadOnDOFs))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

nodes.calculateNodalReactions(False,1e-7)

refVector= xc.Vector([0, F/2.0, 0])
reactionNodeA= nA.getReaction
ratio1= reactionNodeA.Norm()
reactionNode1= n1.getReaction
ratio2= (reactionNode1-refVector).Norm()
reactionNode2= n2.getReaction
ratio3= (reactionNode2-refVector).Norm()

'''
print("ratio1= ", ratio1)
print("reactionNode1= ", reactionNode1)
print("ratio2= ", ratio2)
print("reactionNode2= ", reactionNode2)
print("ratio3= ", ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
