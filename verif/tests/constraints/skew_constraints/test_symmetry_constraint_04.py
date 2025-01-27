# -*- coding: utf-8 -*-
''' Symmetry constraint trivial test.

See section 4.2 of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
 ISBN 097900490X, 9780979004902.  Page 190 et seq.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)

# Geometry
L= 1 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes.
### Half simply-suppoted beam (symmetry at n2)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(L,0,0)
### Whole simply-suppoted beam.
n11= nodes.newNodeXYZ(0,1,0)
n12= nodes.newNodeXYZ(L,1,0)
n13= nodes.newNodeXYZ(2*L,1, 0)

## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

## Define material.
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
section= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)

## Define elements.
# Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
halfBeam= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
wholeBeamLeft= elements.newElement("ElasticBeam3d",xc.ID([n11.tag,n12.tag]))
wholeBeamRight= elements.newElement("ElasticBeam3d",xc.ID([n12.tag,n13.tag]))

# Constraints
## Half beam constraints.
modelSpace.fixNode('000_0FF', n1.tag)
vertPlane= geom.Plane3d(geom.Pos3d(L,0,0), geom.Pos3d(L,10,0), geom.Pos3d(L,0, 10)) # Vertical plane at beam end.
# Normally, the symmetry constraints will not be aligned with the axes, but
# this case is an exception to make the checking of the results easier.
symmetry= modelSpace.newSymmetryConstraint(n2.tag, vertPlane)
## Whole beam constraints.
modelSpace.fixNode('000_0FF', n11.tag)
modelSpace.fixNode('000_0FF', n13.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
loadVector= xc.Vector([0,0, -F,0, 0, 0])
lp0.newNodalLoad(n2.tag, loadVector)
lp0.newNodalLoad(n12.tag, 2*loadVector)
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solve.
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
    errMsg= 'Solution algorithm failed.'
    lmsg.error(errMsg)
    exit(1)

# Check results.
## Check displacement results (verify symmetry).
errorDisp= (n12.getDisp-n2.getDisp).Norm()

## Check reaction results (verify symmetry).
modelSpace.calculateNodalReactions()

R= n2.getReaction
Mref= -F*L
errorForce= abs(R[4]-Mref)/Mref

'''
print(R)
print(Mref)
print(errorDisp, errorForce)
'''

import os
fname= os.path.basename(__file__)
if (abs(errorDisp)<1e-10) and (abs(errorForce)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uZ')
