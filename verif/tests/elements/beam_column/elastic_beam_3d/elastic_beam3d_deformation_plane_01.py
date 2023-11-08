# -*- coding: utf-8 -*-
''' Trivial test for the DeformationPlane class.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 5.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iz= 80.1e-8 # Cross section moment of inertia (m4)
Iy= Iz # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 10 # Temperature increment (Celsius degrees)

F= 1e4
M= F

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define nodes.
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

# Define materials.
scc= typical_materials.defElasticSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J)

# Elements definition
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag, n2.tag]))
beamSection= beam3d.physicalProperties.getVectorMaterials[0]


# Constraints
modelSpace.fixNode000_000(n1.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,0,0,0,0])) # Axial load.
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

errors= list()
# Get generalized strains.

strains0= beamSection.getDeformationPlane().strains
refEpsilon0= F/E/A
epsilon= beamSection.getSectionDeformationByName('defN') # Axial deformation.
err= (strains0[0]-refEpsilon0)**2+strains0[1]**2+strains0[2]**2+(epsilon-refEpsilon0)**2
errors.append(err)

modelSpace.removeLoadCaseFromDomain(lp0.name)
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([0,0,0,0,0,M])) # Z curvature 
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)

result= analysis.analyze(1)
beam3d.getResistingForce()

strains1= beamSection.getDeformationPlane().strains
refCurvatureZ= M/E/Iz
curvatureZ= beamSection.getSectionDeformationByName('defMz')
err= strains1[0]**2+(strains1[1]-refCurvatureZ)**2+strains1[2]**2+(curvatureZ-refCurvatureZ)**2
errors.append(err)
Mz= beamSection.getStressResultantComponent('Mz')

err= math.sqrt((beam3d.getMz1-M)**2+(beam3d.getMz2-M)**2+(Mz-M)**2) # As a precaution.
errors.append(err)

modelSpace.removeLoadCaseFromDomain(lp1.name)
lp2= modelSpace.newLoadPattern(name= '2')
lp2.newNodalLoad(n2.tag,xc.Vector([0,0,0,0,M,0])) # Y curvature 
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp2.name)

result= analysis.analyze(1)
beam3d.getResistingForce()

strains2= beamSection.getDeformationPlane().strains
refCurvatureY= M/E/Iy
curvatureY= beamSection.getSectionDeformationByName('defMy')
err= strains2[0]**2+strains2[1]**2+(strains2[2]-refCurvatureY)**2+(curvatureY-refCurvatureY)**2
errors.append(err)
My= beamSection.getStressResultantComponent('My')

err= math.sqrt((beam3d.getMy1-M)**2+(beam3d.getMy2-M)**2+(My-M)**2) # As a precaution.
errors.append(err)

'''
print('strains1= ', strains1)
print('refCurvatureZ=', refCurvatureZ)
print('curvatureZ=', curvatureZ)
print('\nMz= ',Mz)
print('Mz1= ',beam3d.getMz1)
print('Mz2= ',beam3d.getMz2)
print('\nstrains2= ', strains2)
print('refCurvatureY=', refCurvatureY)
print('curvatureY =', curvatureY)
print('\nMy= ',My)
print('My1= ',beam3d.getMy1)
print('My2= ',beam3d.getMy2)
print(errors)
'''

err= max(errors)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# oh.displayLoads()
# #oh.displayReactions()
# oh.displayIntForcDiag('Mz')
