# -*- coding: utf-8 -*-
''' Test displaying strain loads on Elastic Beam 2D elements. Home made test.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
I= (2e-2)**4/12 # Cross section moment of inertia (m4)
AT= 10 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)
n11= nodes.newNodeXY(0,L)
n12= nodes.newNodeXY(L,L)
n21= nodes.newNodeXY(0,2*L)
n22= nodes.newNodeXY(L,2*L)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
elements.dimElem= 2 # Dimension of element space
beam2dA= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beam2dB= elements.newElement("ElasticBeam2d",xc.ID([n11.tag,n12.tag]))
beam2dC= elements.newElement("ElasticBeam2d",xc.ID([n21.tag,n22.tag]))
    
# Constraints
for n in [n1, n2, n11, n12, n21, n22]:
    modelSpace.fixNode('000', n.tag)

# Loads definition

# Load case definition.
factor= 0.85
lp0= modelSpace.newLoadPattern(name= '0')
lp0.gammaF= factor
eleLoadA= lp0.newElementalLoad("beam_strain_load")
eleLoadA.elementTags= xc.ID([beam2dA.tag, beam2dC.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoadA.backEndDeformationPlane= thermalDeformation
eleLoadA.frontEndDeformationPlane= thermalDeformation

eleLoadB= lp0.newElementalLoad("beam_strain_load")
eleLoadB.elementTags= xc.ID([beam2dB.tag, beam2dC.tag])
zCurvature= 1e3/(E*I)
gradientDeformation= xc.DeformationPlane(xc.Vector([0.0, -zCurvature/2.0])) # deformation for point (0,1)= curvature/2.0
eleLoadB.backEndDeformationPlane= gradientDeformation
eleLoadB.frontEndDeformationPlane= gradientDeformation

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in ['epsilon', 'kappa']:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'kappa', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
