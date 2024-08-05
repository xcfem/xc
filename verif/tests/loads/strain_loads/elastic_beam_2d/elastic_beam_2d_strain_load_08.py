# -*- coding: utf-8 -*-
''' Deflection of a simply supported beam due to a temperature gradient. 
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

### K-points.
p1= modelSpace.newKPoint(0.0,0.0)
p2= modelSpace.newKPoint(L/2.0,0.0)
p3= modelSpace.newKPoint(L,0.0)
### Lines.
l1= modelSpace.newLine(p1, p2)
l2= modelSpace.newLine(p2, p3)

### Seed element.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
#### Material.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
modelSpace.setDefaultMaterial(scc)
#### Elements.
beam1= modelSpace.newSeedElement(elementType= "ElasticBeam2d")
beam1.h= h

### Generate mesh.
l1.genMesh(xc.meshDir.I)
l2.genMesh(xc.meshDir.I)
n1= p1.getNode()
n2= p2.getNode()
n3= p3.getNode()

# Sets.
xcTotalSet= modelSpace.getTotalSet()
# Beam elements tags.
beamElementTags= list()
for e in xcTotalSet.elements:
    beamElementTags.append(e.tag)

### Constraints
modelSpace.fixNode('00F', n1.tag)
modelSpace.fixNode('F0F', n3.tag)


## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID(beamElementTags)
temperatureGradient= 13.2 # temperature gradient Ttop-Tbottom.
zCurvature= -temperatureGradient*alpha/h
# We introduce the "inverse" curvature as initial deformation to
# get the desired effect.
deformation= xc.DeformationPlane(xc.Vector([0.0, zCurvature/2.0])) # deformation for point (0,1)= curvature/2.0
eleLoad.backEndDeformationPlane= deformation
eleLoad.frontEndDeformationPlane= deformation
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

result= modelSpace.analyze(calculateNodalReactions= True)


yDeflection= n2.getDisp[1]
yDeflectionRef= -temperatureGradient*alpha*L**2/8.0/h# Deflection of a cantilever due to a temperature gradient.
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
if(abs(ratio)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayReactions()
# # oh.displayIntForcDiag('N', defFScale= 2.0)
# # oh.displayIntForcDiag('M')
# # oh.displayDispRot('uX', defFScale= 100.0)
# oh.displayDispRot('uY', defFScale= 100.0)
# # oh.displayDispRot('rotZ', defFScale= 100.0)
