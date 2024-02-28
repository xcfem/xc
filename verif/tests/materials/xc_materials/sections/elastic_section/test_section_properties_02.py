# -*- coding: utf-8 -*-
'''Check that section materials keep their user defined properties when 
  assigned to elements that use only the mass properties of the section and
  not the section itself (for example elastic beam 3D elements).'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Problem geometry
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(1,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Section geometry
scc10x20= section_properties.RectangularSection(name="rectang",b=.10,h=.20) # Section geometry.
# Section material
matscc10x20= typical_materials.MaterialData(name='mtrectang',E=2.1e6,nu=0.3,rho=2500) # Section material.

# Material definition
matPoteau= scc10x20.defElasticSection3d(preprocessor,matscc10x20)
matPoteau.setProp('Test', 2.71828)

propNamesA= matPoteau.getPropNames()

elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= matPoteau.name
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

propNamesB= beam3d.physicalProperties.getVectorMaterials[0].getPropNames()

testOK= (propNamesA==propNamesB)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

