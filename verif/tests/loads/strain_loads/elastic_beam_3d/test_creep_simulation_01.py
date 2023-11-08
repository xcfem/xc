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

#  n1     L
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
# Define nodes.
n1= nodes.newNodeXYZ(0,L,0)
n2= nodes.newNodeXYZ(L,L,0)
n3= nodes.newNodeXYZ(L,0,0)

# Define materials.
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)

# Elements definition
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
beam= modelSpace.newElement("ElasticBeam3d",[n1.tag, n2.tag])
column= modelSpace.newElement("ElasticBeam3d",[n3.tag, n2.tag])

# Constraints
modelSpace.fixNode000_000(n1.tag)
modelSpace.fixNode000_000(n3.tag)
    
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
beamMz1a= beam.getMz1
beamMy1a= beam.getMy1
beamN2a= beam.getN2
column.getResistingForce()
columnN1a= column.getN1
columnN2a= column.getN2

# Get generalized strains.
beamStrains= beam.physicalProperties.getVectorMaterials[0].getSectionDeformationPlane().strains
columnStrains= beam.physicalProperties.getVectorMaterials[0].getDeformationPlane().strains

print(beamMz1a)
print(beamMy1a)
print(beamStrains)
print(columnStrains)

'''
N= (-E*A*alpha*AT)
ratio= ((axil2-N)/N)

print("N= ",N/1e3)
print("axil1= ",axil1/1e3)
print("axil2= ",axil2/1e3)
print("ratio= ",ratio)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
'''

# Graphic stuff
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
oh.displayFEMesh()
#oh.displayReactions()
oh.displayIntForcDiag('N', defFScale= 50.0)
