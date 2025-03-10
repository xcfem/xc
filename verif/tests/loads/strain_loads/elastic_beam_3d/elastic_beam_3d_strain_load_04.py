# -*- coding: utf-8 -*-
''' Deflection of a cantilever beam due to a temperature gradient. 
    Home made test based on the corresponding classical formulas.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
h= 2e-2 # cross section depth (m)
A= h*h # bar area expressed in square meters.
Iz= 80.1e-8 # Cross section moment of inertia (m4)
Iy= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 13.2 # Temperature gradient (Celsius degrees)
alpha= 1.2e-5 # Thermal expansion coefficient of the steel


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Geometry.

### Nodes.
n1= nodes.newNodeXYZ(0.0,0,0)
n2= nodes.newNodeXYZ(L/2.0,0,0)
n3= nodes.newNodeXYZ(L,0,0)

### Elements.
#### Define materials.
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)
modelSpace.setDefaultMaterial(scc)
#### Define geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(lin)
#### Elements.
beam1= modelSpace.newElement(elementType= "ElasticBeam3d", nodeTags= [n1.tag, n2.tag])
beam2= modelSpace.newElement(elementType= "ElasticBeam3d", nodeTags= [n2.tag, n3.tag])

# Constraints
modelSpace.fixNode('000_000',n1.tag)

## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam1.tag, beam2.tag])
yCurvature= -AT*alpha/h
# We introduce the "inverse" curvature as initial deformation to
# get the desired effect.
deformation= xc.DeformationPlane(xc.Vector([0.0, 0.0, -yCurvature/2.0]))
eleLoad.backEndDeformationPlane= deformation
eleLoad.frontEndDeformationPlane= deformation
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve
result= modelSpace.analyze(calculateNodalReactions= False)

# Check results.
zDeflection= n3.getDisp[2]
zDeflectionRef= AT*alpha*L**2/2.0/h# Deflection of a cantilever due to a temperature gradient.
ratio= abs(zDeflection-zDeflectionRef)/zDeflectionRef

'''
print(n3.getDisp)
print("curvature= ", yCurvature)
print("zDeflection= ", zDeflection*1e3, 'mm')
print("zDeflectionRef= ", zDeflectionRef*1e3, 'mm')
print("ratio= ", ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayReactions()
# # oh.displayIntForcDiag('N', defFScale= 2.0)
# oh.displayDispRot('uY', defFScale= 1000.0)
