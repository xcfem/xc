# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under uniform vertical load along 
its length.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
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
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)

lin= modelSpace.newLinearCrdTransf("lin")

# Materials
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag, n2.tag]))

modelSpace.fixNode000(n2.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
crdTransf= beam2d.getCoordTransf
vIElem= crdTransf.getIVector
vJElem= crdTransf.getJVector
loadVector= -(F/L)*vJElem
beam2d.vector2dUniformLoadGlobal(loadVector)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

delta= n1.getDisp[1] # ydisplacement of node 2.
deltateor= (-F*L**3/(8*E*Iz))
ratio1= abs(delta-deltateor)/deltateor

beam2d.getResistingForce()
M= beam2d.getM2
MTeor= -F*L/2.0
ratio2= abs((M-MTeor)/MTeor)

V= beam2d.getV

ratio3= abs(V-(F/2))/(F/2)

# Check getM1 and getM2 (LP 28/04/2024).
M1= beam2d.getM1
M2Ref= MTeor
M2= beam2d.getM2
ratio4= math.sqrt((M1)**2+(M2-M2Ref)**2)

# Check getV1 and getV2 (LP 28/04/2024).
V1= beam2d.getV1
V2Ref= F
V2= beam2d.getV2
ratio5= math.sqrt((V1)**2+(V2-V2Ref)**2)

'''
print('delta= ', delta)
print('refDelta= ', deltateor)
print('ratio1= ', ratio1)
print("M= ",M)
print("MTeor= ",MTeor)
print('ratio2= ', ratio2)
print('F= ', F)
print("V= ", V)
print('ratio3= ', ratio3)
print('M2Ref= ', M2Ref/1e3, 'M1= ', M1/1e3, ' M2= ', M2/1e3, ' ratio4= ', ratio4)
print('V2Ref= ', V2Ref/1e3, 'V1= ', V1/1e3, ' V2= ', V2/1e3, ' ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 5.0)
# oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayIntForcDiag('N')
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')
# #oh.displayLocalAxes()
