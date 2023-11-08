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

#  n1     5*L
#   |----------+ n2
#              |
#              | 
#              | L
#              |
#              | 
#             --- n3
#

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes.
n1= nodes.newNodeXYZ(0,L,0)
n2= nodes.newNodeXYZ(5*L,L,0)
n3= nodes.newNodeXYZ(5*L,0,0)

## Define elements.
### Define element materials.
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A= A, E= E, G= G,Iz= Iz, Iy= Iy, J= J)
modelSpace.setDefaultMaterial(scc)

### Define element coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(lin)

### Define elements themselves.
beam= modelSpace.newElement("ElasticBeam3d",[n1.tag, n2.tag])
column= modelSpace.newElement("ElasticBeam3d",[n3.tag, n2.tag])

# Constraints
modelSpace.fixNode('000_000', n1.tag)
modelSpace.fixNode('000_000', n3.tag)
    
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
beamMz1a= beam.getMz1
beamMz2a= beam.getMz2
beamMy1a= beam.getMy1
beamMy2a= beam.getMy2
column.getResistingForce()
columnN1a= column.getN1
columnN2a= column.getN2
columnMz1a= column.getMz1
columnMz2a= column.getMz2
columnMy1a= column.getMy1
columnMy2a= column.getMy2

# Get generalized strains.
beamStrains= beam.physicalProperties.getVectorMaterials[0].getDeformationPlane().strains
columnStrains= column.physicalProperties.getVectorMaterials[0].getDeformationPlane().strains
columnZCurvature= columnStrains[1]

# Assume that creep removes the stresses due to bending in the column.

## Load case definition.
creep= modelSpace.newLoadPattern(name= 'creep')
factor= 0.87835325
eleLoad= creep.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([column.tag])
creepDeformation= xc.DeformationPlane(xc.Vector([0.0, -factor*columnZCurvature, 0.0]))
eleLoad.backEndDeformationPlane= creepDeformation
eleLoad.frontEndDeformationPlane= creepDeformation
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(creep.name)

result= modelSpace.analyze(calculateNodalReactions= False)

column.getResistingForce()
columnMz1b= column.getMz1
columnMz2b= column.getMz2

# Check result
ratio= abs(columnMz2b)/columnMz2a

'''
print('before beam Mz1= ', beamMz1a/1e3)
print('before beam Mz2= ', beamMz2a/1e3)
print('before column Mz1= ', columnMz1a/1e3)
print('before column Mz2= ', columnMz2a/1e3)
print('before beam epsilon= ', beamStrains[0], 'beam z curvature: ', beamStrains[1], 'beam y curvature: ', beamStrains[2])
print('before column epsilon= ', columnStrains[0], 'column z curvature: ', columnStrains[1], 'column y curvature: ', columnStrains[2])
print('factor= ', factor)
print('after column Mz1= ', columnMz1b/1e3)
print('after column Mz2= ', columnMz2b/1e3)
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
# oh.displayIntForcDiag('Mz', defFScale= 50.0)

