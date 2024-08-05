# -*- coding: utf-8 -*-
''' Comparison of the internal forces of a beam with a temperature gradient
    and a beam subject to the equivalent moment (the moment that produces the
    same curvature).
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
h= 0.25
A= h*h # bar area expressed in square meters
I= h**4/12.0 # Cross section moment of inertia (m4)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Geometry.

### Nodes.
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)
n3= nodes.newNodeXY(0,4.0)
n4= nodes.newNodeXY(L,4.0)

### Elements.
#### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
#### Material.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
modelSpace.setDefaultMaterial(scc)
#### Elements.
beam1= modelSpace.newElement(elementType= "ElasticBeam2d", nodeTags= [n1.tag, n2.tag])
beam1.h= h
beam2= modelSpace.newElement(elementType= "ElasticBeam2d", nodeTags= [n3.tag, n4.tag])
beam2.h= h

### Constraints
modelSpace.fixNode('000', n1.tag)
modelSpace.fixNode('000', n2.tag)
modelSpace.fixNode('00F', n3.tag)
modelSpace.fixNode('F0F', n4.tag)


M= 1e3
zCurvature= M/scc.EIz()

## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam1.tag])
# We introduce the same curvature as initial deformation to
# get the desired effect.
deformation= xc.DeformationPlane(xc.Vector([0.0, zCurvature/2])) # deformation for point (0,1)= curvature/2.0
eleLoad.backEndDeformationPlane= deformation
eleLoad.frontEndDeformationPlane= deformation
nodalLoad= lp0.newNodalLoad(n3.tag, xc.Vector([0.0,0.0,-M]) )
nodalLoad= lp0.newNodalLoad(n4.tag, xc.Vector([0.0,0.0,M]) )
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

result= modelSpace.analyze(calculateNodalReactions= False)

beam1.getResistingForce()
beam1M1= beam1.getM1
beam1M2= beam1.getM2
beam1M= beam1.getM
beam2.getResistingForce()
beam2M1= beam2.getM1
beam2M2= beam2.getM2
beam2M= beam2.getM

ratio1= abs(beam1M1-beam2M1)/beam2M1
ratio2= abs(beam1M2-beam2M2)/beam2M2
ratio3= abs(beam1M-beam2M)/beam2M

'''
print('\nbeam 1 M1= ', beam1M1)
print('beam 1 M2= ', beam1M2)
print('beam 1 M= ', beam1M)
print('\nbeam 2 M1= ', beam2M1)
print('beam 2 M2= ', beam2M2)
print('beam 2 M= ', beam2M)

print('M= ', M/1e3, ' kN.m')
print("curvature= ", zCurvature)
print("ratio1= ", ratio1)
print("ratio2= ", ratio2)
print("ratio3= ", ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayReactions()
# # oh.displayIntForcDiag('N', defFScale= 2.0)
# oh.displayIntForcDiag('M', defFScale= 2.0)
# # oh.displayDispRot('uY', defFScale= 10.0)
