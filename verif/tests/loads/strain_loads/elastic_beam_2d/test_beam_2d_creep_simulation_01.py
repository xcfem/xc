# -*- coding: utf-8 -*-
''' Simulate creep by introducing an imposed strain at the element materials.
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
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iz= 80.1e-8 # Cross section moment of inertia (m4)
Iy= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 100 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

#  n1    5*L
#   |----------+ n2
#              |
#              | 
#              | L
#              |
#              | 
#             --- n3
#

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define nodes.
n1= nodes.newNodeXY(0,L)
n2= nodes.newNodeXY(5*L,L)
n3= nodes.newNodeXY(5*L,0)

# Define materials.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A= A, E=E, I= Iz)
modelSpace.setDefaultMaterial(scc)

# Define element coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)

# Elements definition
beam= modelSpace.newElement("ElasticBeam2d",[n1.tag, n2.tag])
column= modelSpace.newElement("ElasticBeam2d",[n3.tag, n2.tag])

# Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNode000(n3.tag)
    
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
#\set_current_load_pattern{"0"}
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam.tag, column.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Get internal forces.
beam.getResistingForce()
beamN1a= beam.getN1
beamN2a= beam.getN2
beamM1a= beam.getM1
beamM2a= beam.getM2
column.getResistingForce()
columnN1a= column.getN1
columnN2a= column.getN2
columnM1a= column.getM1
columnM2a= column.getM2

# Get generalized strains.
columnStrains= column.physicalProperties.getVectorMaterials[0].getDeformationPlane().strains
columnZCurvature= columnStrains[1]


# Assume that creep removes the stresses due to bending in the column.

## Load case definition.
creep= modelSpace.newLoadPattern(name= 'creep')
factor= 0.87835325 # This value has no other meaning than reaching an almost zero
                   # bending moment in the front end of the column.
eleLoad= creep.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([column.tag])
### set imposed strain: [epsilon, zCurvature]
creepDeformation= xc.DeformationPlane(xc.Vector([0.0, -factor*columnZCurvature]))
eleLoad.backEndDeformationPlane= creepDeformation
eleLoad.frontEndDeformationPlane= creepDeformation
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(creep.name)

result= modelSpace.analyze(calculateNodalReactions= False)

column.getResistingForce()
columnM1b= column.getM1
columnM2b= column.getM2

# Check result
ratio= abs(columnM2b)/columnM2a

'''
print('before beam M1= ', beamM1a/1e3)
print('before beam M2= ', beamM2a/1e3)
print('before column M1= ', columnM1a/1e3)
print('before column M2= ', columnM2a/1e3)
print('before column epsilon= ', columnStrains[0], 'column z curvature: ', columnStrains[1])
print('factor= ', factor)
print('after column M1= ', columnM1b/1e3)
print('after column M2= ', columnM2b/1e3)
print(ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# #oh.displayReactions()
# oh.displayIntForcDiag('N', defFScale= 50.0)
# oh.displayIntForcDiag('M', defFScale= 50.0)
