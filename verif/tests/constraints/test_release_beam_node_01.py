# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from model import predefined_spaces

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Release beam end test.'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

# Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W18X50')
xcSection= shape.defElasticShearSection3d(preprocessor)

# Model geometry

## Points.
span= 10.0
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPntFromPos3d(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 1

# Mesh
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Release last element.
elemToRelease= xcTotalSet.getNearestElement(p1.getPos)
(newNodes, newElements)= modelSpace.releaseBeamEnd(elemToRelease, stiffnessFactors= [0.0,1.0e9,1.0e9,1.0e9,1e9,1.0e9], nodesToRelease= [0])


# Constraints
n0= p0.getNode()
n1= p1.getNode()
modelSpace.fixNode('000_000',n0.tag)
modelSpace.fixNode('000_000',n1.tag)

# Check that n0 is no longer connected to the node
index= elemToRelease.getNodes.getNodeIndex(n0)
ratio0= (index+1)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
f= 1e3
for e in l1.elements:
    e.vector3dUniformLoadGlobal(xc.Vector([f,0.0,0.0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
modelSpace.analyze(calculateNodalReactions= True)


ratio1= n0.getReaction[0]
ratio2= abs(n1.getReaction[0]+f*l1.getLength())/(f*l1.getLength())
ratio3= newNodes[0].getReaction[0]

'''
print(ratio0)
print(' reaction node ', n0.tag,'= ', n0.getReaction)
print(' reaction node ', n1.tag,'= ', n1.getReaction)

for n in newNodes:
    print(' reaction node ', n.tag,'= ', n.getReaction)
print(' ratio1= ', ratio1)
print(' ratio2= ', ratio2)
print(' ratio3= ', ratio3)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-15) & (ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
