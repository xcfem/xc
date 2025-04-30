# -*- coding: utf-8 -*-
''' Test displaying strain loads on Elastic Beam 3D elements. Home made test.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 10 # Temperature increment (Celsius degrees)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)
n11= nodes.newNodeXYZ(0,0,L)
n12= nodes.newNodeXYZ(L,0,L)
n21= nodes.newNodeXYZ(0,0,2*L)
n22= nodes.newNodeXYZ(L,0,2*L)

# Define geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
section= typical_materials.defElasticShearSection3d(preprocessor, "section",A,E,G,Iz,Iy,J, alpha_y= 1, alpha_z= 1)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam3dA= elements.newElement("ForceBeamColumn3d",xc.ID([n1.tag, n2.tag]))
beam3dB= elements.newElement("ForceBeamColumn3d",xc.ID([n11.tag,n12.tag]))
beam3dC= elements.newElement("ForceBeamColumn3d",xc.ID([n21.tag,n22.tag]))
    
# Constraints
for n in [n1, n2, n11, n12, n21, n22]:
    modelSpace.fixNode('000_000', n.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
## Elongation
eleLoadA= lp0.newElementalLoad("beam_strain_load")
eleLoadA.elementTags= xc.ID([beam3dA.tag, beam3dC.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoadA.backEndDeformationPlane= thermalDeformation
eleLoadA.frontEndDeformationPlane= thermalDeformation
## Curvature
eleLoadB= lp0.newElementalLoad("beam_strain_load")
eleLoadB.elementTags= xc.ID([beam3dB.tag, beam3dC.tag])
yCurvature= 1e3/(E*Iy)
gradientDeformation= xc.DeformationPlane(xc.Vector([0.0, 0.0, -yCurvature/2.0])) # deformation for point (0,1)= curvature/2.0
eleLoadB.backEndDeformationPlane= gradientDeformation
eleLoadB.frontEndDeformationPlane= gradientDeformation

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in ['epsilon', 'kappa_y']:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'kappa_y', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
