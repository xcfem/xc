# -*- coding: utf-8 -*-
''' Home made test.  Check element release property.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties (IPE-80)
A= 7.64e-4 # Cross section area (m2)
Iz= 8.49e-8 # Cross section moment of inertia (m4)

# Geometry
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)

n11= nodes.newNodeXY(0,1)
n12= nodes.newNodeXY(L,1)

n21= nodes.newNodeXY(0,2)
n22= nodes.newNodeXY(L,2)

lin= modelSpace.newLinearCrdTransf("lin")

# Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Define element.
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam2dA= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
beam2dB= elements.newElement("ElasticBeam2d",xc.ID([n11.tag,n12.tag]))
beam2dC= elements.newElement("ElasticBeam2d",xc.ID([n21.tag,n22.tag]))

# Release code; 0: no release, 1: node I, 2: node J, 2: both nodes.
beam2dA.release= 1 # node I.
beam2dB.release= 2 # node J.
beam2dC.release= 3 # both nodes.

# Constraints.
for n in [n1, n2, n11, n12, n21, n22]:
    modelSpace.fixNode000(n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadVector= xc.Vector([0, -F/L])
beam2dA.vector2dUniformLoadGlobal(loadVector)
beam2dB.vector2dUniformLoadGlobal(loadVector)
beam2dC.vector2dUniformLoadGlobal(loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

M1a= beam2dA.getM1
M2a= beam2dA.getM2
M1b= beam2dB.getM1
M2b= beam2dB.getM2
M1c= beam2dC.getM1
M2c= beam2dC.getM2

err= math.sqrt(M1a**2+(M2a-M1b)**2+M2b**2+M1c**2+M2c**2)

'''
print('M1a= ', M1a)
print('M2a= ', M2a)
print('M1b= ', M1b)
print('M2b= ', M2b)
print('M1c= ', M1c)
print('M2c= ', M2c)
print(err)
'''

testOK= (abs(err)<1e-6) and (abs(M2a)>0.0)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayIntForcDiag('M')
# # oh.displayIntForcDiag('M')
# # oh.displayLocalAxes()
