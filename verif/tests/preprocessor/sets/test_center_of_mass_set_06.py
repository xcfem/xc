# -*- coding: utf-8 -*-
'''Compute the center of mass of a set.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodeHandler)
nodeMass= 1e3
nodeMassMatrix= xc.Matrix([[nodeMass, 0, 0], [0, nodeMass, 0], [0, 0, 0]])

# Define nodes.
nodes= list()
refNodeCenterOfMass= geom.Pos3d(0, 0, 0)
for x in range(0, 10):
    refNodeCenterOfMass+= geom.Vector3d(x, 0, 0)
    newNode= modelSpace.newNode(x, 0)
    newNode.mass= nodeMassMatrix
    nodes.append(newNode)
factor= 1/len(nodes)
refNodeCenterOfMass.x*= factor
refNodeCenterOfMass.y*= factor
refNodeCenterOfMass.z*= factor

# Define elements.
## Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties
A= 7.64e-4 # Cross section area (m2)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
## Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
elementRho= 1e3
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties, overrideRho= elementRho)
## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")
## Define elements.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(section)
refElementCenterOfMass= geom.Pos3d(0, 0, 0)
elements= list()
n0= nodes[0]
for n1 in nodes[1:]:
    newElement= modelSpace.newElement("ElasticBeam2d", [n0.tag, n1.tag])
    centroid= newElement.getPosCentroid(True)
    refElementCenterOfMass+= geom.Vector3d(centroid.x, 0, 0)
    elements.append(newElement)
    n0= n1
factor= 1/len(elements)
refElementCenterOfMass.x*= factor
refElementCenterOfMass.y*= factor
refElementCenterOfMass.z*= factor


# Check results.
xcTotalSet= modelSpace.getTotalSet()
totalMass= xcTotalSet.totalMass
elementMassMatrix= elements[0].totalMass
elementTotalMass= len(elements)*elementMassMatrix(0,0)
nodeTotalMass= len(nodes)*nodeMassMatrix(0, 0)
org= geom.Pos3d(0,0,0)
refCenterOfMass= org+(refNodeCenterOfMass-org)*(nodeTotalMass/totalMass(0,0))+(refElementCenterOfMass-org)*(elementTotalMass/totalMass(0,0))

refTotalMass= len(nodes)*nodeMassMatrix+len(elements)*elementMassMatrix
ratio1= (totalMass-refTotalMass).Norm()/refTotalMass.Norm()
centerOfMass= xcTotalSet.getCenterOfMass(True)
ratio2= (centerOfMass-refCenterOfMass).getModulus()/refCenterOfMass.x
refCenterOfMassX= nodes[-1].getInitialPos3d.x/2.0
ratio3= (centerOfMass.x-refCenterOfMassX)/refCenterOfMassX

# print("totalMass= ", totalMass)
# print("refTotalMass= ", refTotalMass)
# print("ratio1= ", ratio1)
# print("centerOfMass= ", centerOfMass)
# print("refCenterOfMass= ", refCenterOfMass)
# print("ratio2= ", ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) and (ratio2<1e-15) and (ratio3<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
