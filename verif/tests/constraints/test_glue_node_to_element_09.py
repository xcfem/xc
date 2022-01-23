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
Iy= 2810e-8 # Inertia of the beam section.
Iz= Iy
J= 25.2935e-8 # Cross section torsion constant (m4)

l= 10 # Bar length in inches
F= 1000.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem geometry
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(l,0,0)
nA= nodes.newNodeXYZ(l/2.0,0,0) # node to be glued.

# Materials definition
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
scc= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "scc",sectionProperties)

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

## Element definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= scc.name
elements.defaultTransformation= lin.name
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000_0FF(n1.tag)
modelSpace.fixNode000_FFF(n2.tag)

## Glued node.
gluedDOFs= [0,1,2]
loadOnDOFs= [0,-F,0,0,0,0]
for i in range(0,6):
  if i not in gluedDOFs:
    modelSpace.constraints.newSPConstraint(nA.tag,i,0.0)

glue= modelSpace.constraints.newGlueNodeToElement(nA,beam3d,xc.ID(gluedDOFs))

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nA.tag,xc.Vector(loadOnDOFs))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

nodes.calculateNodalReactions(False,1e-7)

refVector= xc.Vector([0, F/2.0, 0, 0, 0, 0])
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
