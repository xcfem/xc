# -*- coding: utf-8 -*-
''' Check strain load representation on mixed elements. Loaded and unloaded
    elements.
'''

from __future__ import print_function
from __future__ import division

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

L= 1.0 # Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
h= 2e-2
A= h*h # bar area expressed in square meters
I= (h)**4/12 # Cross section moment of inertia (m4)
AT= 10.0 # Temperature increment (Celsius degrees)
# Beam.
Iy= I # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)
n3= nodes.newNodeXYZ(L,h,0.0)
n4= nodes.newNodeXYZ(0,h,0.0)
n11= nodes.newNodeXYZ(0,0,L)
n12= nodes.newNodeXYZ(L,0,L)

# Materials definition
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E,0.3,0.0,h)
elasticBeamSection= typical_materials.defElasticSection3d(preprocessor, "elasticBeamSection",A,E,G,Iz,Iy,J)

# Define geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))


# Elements definition
modelSpace.setDefaultMaterial(shellMat)
shellElement= modelSpace.newElement("ShellMITC4",[n1.tag,n2.tag,n3.tag,n4.tag])
modelSpace.setDefaultMaterial(elasticBeamSection)
modelSpace.setDefaultCoordTransf(lin)
elasticBeam= modelSpace.newElement("ElasticBeam3d",[n11.tag,n12.tag])

# Constraints
for n in [n1, n2, n3, n4, n11, n12]:
    modelSpace.fixNode('000_000', n.tag)

# Load case definition.
factor= 0.85
lp0= modelSpace.newLoadPattern(name= '0')
lp0.gammaF= factor
shellEleLoad= lp0.newElementalLoad("shell_strain_load")
shellEleLoad.elementTags= xc.ID([shellElement.tag])
shellEleLoad.setStrainComp(0,0,alpha*AT) #(id of Gauss point, id of component, value)
shellEleLoad.setStrainComp(1,0,alpha*AT)
shellEleLoad.setStrainComp(2,0,alpha*AT)
shellEleLoad.setStrainComp(3,0,alpha*AT)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in modelSpace.shellStrainComponents:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'epsilon_1', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
