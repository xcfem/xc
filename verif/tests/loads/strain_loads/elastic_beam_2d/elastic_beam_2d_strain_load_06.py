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

L= 10.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
h= 0.25 # section depth
A= h*h # bar area expressed in square meters
I= h**4/12.0 # Cross section moment of inertia (m4)
alpha= 1e-5 # Thermal expansion coefficient.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Geometry.

### Nodes.
n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(L/2.0,0.0)
n3= nodes.newNodeXY(L,0.0)

### Elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
#### Material.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
modelSpace.setDefaultMaterial(scc)
#### Elements.
beam1= modelSpace.newElement(elementType= "ElasticBeam2d", nodeTags= [n1.tag, n2.tag])
beam2= modelSpace.newElement(elementType= "ElasticBeam2d", nodeTags= [n2.tag, n3.tag])

### Constraints
modelSpace.fixNode('000', n1.tag)


## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam1.tag, beam2.tag])
temperatureGradient= 13.2 # 
zCurvature= -temperatureGradient*alpha/h
# We introduce the "inverse" curvature as initial deformation to
# get the desired effect.
deformation= xc.DeformationPlane(xc.Vector([0.0, -zCurvature]))
eleLoad.backEndDeformationPlane= deformation
eleLoad.frontEndDeformationPlane= deformation
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

result= modelSpace.analyze(calculateNodalReactions= False)


yDeflection= n3.getDisp[1]
yDeflectionRef= -temperatureGradient*alpha*L**2/2.0/h# Deflection of a cantilever due to a temperature gradient.
ratio= abs(yDeflection-yDeflectionRef)/yDeflectionRef

'''
print("curvature= ", zCurvature)
print("yDeflection= ", yDeflection*1e3, 'mm')
print("yDeflectionRef= ", yDeflectionRef*1e3, 'mm')
print("ratio= ", ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-8):
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
