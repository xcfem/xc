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

L= 1.0 # Bar length (m)
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
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Define nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)

# Define materials.
scc= typical_materials.defElasticSection2d(preprocessor, "scc", A= A, E= E, I= Iz)

# Elements definition
lin= modelSpace.newLinearCrdTransf("lin")
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
elements.dimElem= 2 # Dimension of element space
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag, n2.tag]))
    
# Constraints
modelSpace.fixNode000(n1.tag)

analysis= predefined_solutions.simple_static_linear(feProblem)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


result= analysis.analyze(1)

beamSection= beam2d.physicalProperties.getVectorMaterials[0]

# Get generalized strains.
strains0= beamSection.getDeformationPlane().strains
refEpsilon= F/E/A
epsilon= beamSection.getSectionDeformationByName('defN')
err0= math.sqrt((strains0[0]-refEpsilon)**2+strains0[1]**2+strains0[2]**2+(epsilon-refEpsilon)**2)

modelSpace.removeLoadCaseFromDomain(lp0.name)

lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(n2.tag,xc.Vector([0,0,M]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp1.name)

result= analysis.analyze(1)
beam2d.getResistingForce()


# Get generalized strains.
strains1= beamSection.getDeformationPlane().strains
refCurvature= M/E/Iz
curvature= beamSection.getSectionDeformationByName('defMz')
Mz= beamSection.getStressResultantComponent('Mz')

err1= math.sqrt(strains1[0]**2+(strains1[1]-refCurvature)**2+strains1[2]**2+(curvature-refCurvature)**2)

err2= math.sqrt((beam2d.getM1-M)**2+(beam2d.getM2-M)**2+(Mz-M)**2) # As a precaution.

err= max(err0, err1, err2)

'''
print(strains0, refEpsilon, epsilon, 'err0= ', math.sqrt(err0))
print(strains1, refCurvature, curvature, 'err1= ', err1)
print(beam2d.getM1, beam2d.getM2, Mz, 'err2= ', err2)
print(err)
'''

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
# oh.displayIntForcDiag('M')
