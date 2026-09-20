# -*- coding: utf-8 -*-
'''Change the material of an ElasticBeam3d element.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes
n1= modelSpace.newNode(0, 0, 0)
n2= modelSpace.newNode(1, 0, 0)

# Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials with dummy properties that are used only to identify the material.
sectionA= typical_materials.defElasticSection3d(preprocessor, "sectionA",1,1,1,1,1,1)
sectionB= typical_materials.defElasticSection3d(preprocessor, "sectionB",2,2,2,2,2,2)

## Define element using section A.
modelSpace.setDefaultMaterial(sectionA)
modelSpace.setDefaultCoordTransf(lin)
beam3d= modelSpace.newElement("ElasticBeam3d", [n1.tag,n2.tag])

## Change the element material to section B.
beam3d.setMaterial(sectionB)

beamMaterial= beam3d.physicalProperties.getVectorMaterials[0]

testOK= (beamMaterial.EA()==sectionB.EA()) and (beamMaterial.EIz()==sectionB.EIz()) and (beamMaterial.EIy()==sectionB.EIy())

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

