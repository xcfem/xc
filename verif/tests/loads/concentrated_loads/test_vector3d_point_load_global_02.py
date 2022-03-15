# -*- coding: utf-8 -*-
'''
vector3d_point_load_local verification test on lines. Home made test.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc_base
import geom
import xc

from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 2.0e11 # Young modulus.
l= 1.44 # Bar length in inches.
A= 31e-4 # Beam cross-section area.
h= math.sqrt(A) # Beam cross-section depth.
I= 2810e-8 # Inertia of the beam section.
Iy= I
Iz= I
srz= 2.42 # Shear coefficient.
Az= A/srz
Ay= A-Az
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, name= "scc",A= A,E= E,G= G,Iy= Iy, Iz= Iz,J= Iy/100.0, alpha_y= Ay/A, alpha_z= Az/A)

# Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(l,0,0))

lines= preprocessor.getMultiBlockTopology.getLines
ln= lines.newLine(pt1.tag,pt2.tag)
ln.nDiv= 3 # No node at center.

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam3d= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
beam3d.h= h

xcTotalSet= preprocessor.getSets.getSet("total")
xcTotalSet.genMesh(xc.meshDir.I)

nA= pt1.getNode()
nB= pt2.getNode()

# Constraints
modelSpace.fixNode000_0FF(nA.tag)
modelSpace.fixNodeF00_FFF(nB.tag)

loadPos= xc.Vector([l/2,0.0,0.0])
loadVector= xc.Vector([0,0,-1])

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")

ln.getElementLayers.vector3dPointLoadGlobal(loadPos,loadVector)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Compute results.
nodes.calculateNodalReactions(True,1e-7)
RA= nA.getReaction
refRA= xc.Vector([0,0,0.5,0,0,0])
RB= nB.getReaction
refRB= refRA

ratio= math.sqrt((RA-refRA).Norm()**2+(RB-refRB).Norm()**2)

'''
print("RA= ",RA)
print("RB= ",RB)
print("ratio= ",ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
