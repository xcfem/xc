# -*- coding: utf-8 -*-
''' Deflection of a cantilever beam with constant curvature along its length. 
    Home made test based on the formula of the deflection of a cantilever
    with a couple moment at the free end.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
A= 4e-4 # bar area expressed in square meters
Iz= 80.1e-8 # Cross section moment of inertia (m4)
Iy= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)


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

n4= nodes.newNodeXYZ(0.0,0.0, 2.0)
n5= nodes.newNodeXYZ(L/2.0,0.0, 2.0)
n6= nodes.newNodeXYZ(L,0.0, 2.0)

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
beam3= modelSpace.newElement(elementType= "ElasticBeam3d", nodeTags= [n4.tag, n5.tag])
beam4= modelSpace.newElement(elementType= "ElasticBeam3d", nodeTags= [n5.tag, n6.tag])

# Constraints
modelSpace.fixNode('000_000', n1.tag)
modelSpace.fixNode('000_000', n4.tag)

## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam1.tag, beam2.tag])
M= 1e3
yCurvature= M/(E*Iy)
# We introduce the "inverse" curvature as initial deformation to
# get the desired effect.
deformation= xc.DeformationPlane(xc.Vector([0.0, 0.0, -yCurvature/2.0])) # deformation for point (0,1)= curvature/2.0
eleLoad.backEndDeformationPlane= deformation
eleLoad.frontEndDeformationPlane= deformation
# Equivalent nodal load at node n6
nodalLoad= lp0.newNodalLoad(n6.tag, xc.Vector([0.0,0.0,0.0, 0.0, M, 0.0]) )
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Check getStrainComponentFromName
strainsMatrix= eleLoad.getElementStrainsMatrix(beam1)
responseId= beam1.getSection(0).getResponseType
componentIndex= modelSpace.getStrainComponentFromName('kappa_y', responseId= responseId)
componentRefValue= -yCurvature/2.0
ratio0= math.sqrt((strainsMatrix(0,componentIndex)-componentRefValue)**2+(strainsMatrix(1,componentIndex)-componentRefValue)**2)

# Solve
result= modelSpace.analyze(calculateNodalReactions= False)

# Check results.
zDeflection3= n3.getDisp[2]
zDeflectionRef= -M*(L**2)/(2.0*E*Iy) # Deflection of a cantilever with a couple moment at the free end.
zDeflection6= n6.getDisp[2]
ratio1= abs(zDeflection3-zDeflectionRef)/zDeflectionRef
ratio2= abs(zDeflection6-zDeflectionRef)/zDeflectionRef

'''
print(n3.getDisp)
print(n6.getDisp)
print("curvature= ", yCurvature)
print("zDeflectionRef= ", zDeflectionRef*1e3, 'mm')
print("zDeflection3= ", zDeflection3*1e3, 'mm')
print("ratio1= ", ratio1)
print("zDeflection6= ", zDeflection6*1e3, 'mm')
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio0)<1e-8) and (abs(ratio1)<1e-8) and (abs(ratio2)<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# oh.displayIntForcDiag('My', defFScale= 2.0)
# oh.displayDispRot('uZ', defFScale= 2.0)

