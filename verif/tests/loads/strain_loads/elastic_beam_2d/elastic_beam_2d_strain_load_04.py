# -*- coding: utf-8 -*-
''' Verifies the cummulative effect of strain loads. Home made test. '''

from __future__ import print_function

import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 10.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
I= (2e-2)**4/12 # Cross section moment of inertia (m4)
AT= 50 # Temperature increment (Celsius degrees)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(L,0.0)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= scc.name
elements.dimElem= 2 # Dimension of element space
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
    
# Constraints
modelSpace.fixNode('000', n1.tag)

# Loads definition

# Heating.
lp0= modelSpace.newLoadPattern(name= 'heating')
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam2d.tag])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation
modelSpace.addLoadCaseToDomain(lp0.name) # We add the load case to domain.

result= modelSpace.analyze(calculateNodalReactions= False)

dispA= n2.getDisp[0]
dispARef= alpha*AT*L

# Cooling.
lp1= modelSpace.newLoadPattern(name= 'cooling')
eleLoad= lp1.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam2d.tag])
thermalDeformation= xc.DeformationPlane(-alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation
modelSpace.addLoadCaseToDomain(lp1.name) # We add the load case to domain.

result= modelSpace.analyze(calculateNodalReactions= False)

dispB= n2.getDisp[0]

# Check results.
ratio1= abs(dispA-dispARef)/dispARef
ratio2= abs(dispB)

'''
print(dispA, ratio1)
print(dispB, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-8 and abs(ratio2)<1e-8:
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

