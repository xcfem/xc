# -*- coding: utf-8 -*-
''' Check FlatSiderSimple2d revertToStart method.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
import math
from materials import friction_bearings as fb
from model import predefined_spaces
from materials import typical_materials
from model import friction_models as fm
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodeHandler)

# 2. Define nodes (zero-height bearing at coordinates x=0, y=0)
L= 30 # Beam length.
nod0= modelSpace.newNode(0,0)
nod1= modelSpace.newNode(L,0)
nod2= modelSpace.newNode(L,0)

# 3. Define Constratints.
modelSpace.fixNode('000', nod0.tag)
modelSpace.fixNode('000', nod1.tag)

# 4. Define beam element.
## 4.1 Define material.
E= 2.1e9
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
b= 0.5 # cross section width (m).
h= b # ross section depth (m).
A= b*h # Cross section area (m2)
Iz= 1/12*b*h**3 # Cross section moment of inertia (m4)
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section", sectionProperties)
## 4.1 Define coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")
## 4.2 Define beam element.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(section)
beam2d= modelSpace.newElement("ElasticBeam2d", [nod0.tag, nod2.tag])

# 5. Define slider bearing.
# 5.1 Define axial and rotational uniaxial materials
# Material for axial direction (axialMat), high stiffness in compression,
# no tension
axialMat= typical_materials.defElastNoTensMaterial(preprocessor, name= "axialMat", E= 1e9)
# Material for rotational/other restrained direction (tag 2)
rotMat= typical_materials.defElasticMaterial(preprocessor, name= "rotMat", E= 1e10)

# 4. Define friction model (coefficient of friction = 0.05)
mu= .05
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= "frictionModel", mu= mu)

# 5.2 Construct flatSliderBearing element (nodes 1->2, 
# combining matTags for axial and rotation, shear via friction model)
kInit= 1e6 # Initial elastic stiffness in local shear direction.
mass= 0.0 # element mass 
flatSliderBearing= fb.def_flat_slider_bearing_2d(modelSpace, n1= nod1, n2= nod2, frictionModel= frictionModel, vertResp= axialMat, rotResp= rotMat, kInit= kInit, x= xc.Vector([0,1,0]), y= xc.Vector([-1,0,0]), shearDistI= 0.0, addRayleigh= 0, mass= mass)

# 6. Create loads.
# 6.1 Gravity load.
lpG= modelSpace.newLoadPattern(name= 'G')
F= 1.5e3 # Load magnitude (kN)
lpG.newNodalLoad(nod2.tag,xc.Vector([0,-F,0]))

# 6.2
lpT= modelSpace.newLoadPattern(name= 'T')
eleLoad= lpT.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam2d.tag])
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 40 # Temperature increment (Celsius degrees)
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation

# # 7. Solution procedure.
numSteps= 1
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-10, printFlag= 0, numSteps= numSteps, numberingMethod= 'simple', convTestType= 'norm_unbalance_conv_test')
solProc.setup()
analysis= solProc.getAnalysis()

# 8. Perform the gravity analysis.
modelSpace.addLoadCaseToDomain(lpG.name)
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Gravity load: can't solve.")
    
modelSpace.calculateNodalReactions()
RG1= nod1.getReaction
RG1x= RG1[0]
RG1y= RG1[1]

# 9. Perform the strain load analysis.
modelSpace.addLoadCaseToDomain(lpT.name)
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Thermal load: can't solve.")

modelSpace.calculateNodalReactions()
RT1= nod1.getReaction
RT1x= RT1[0]
RT1y= RT1[1]

# 10. Check results.
ratio0= abs(RG1x)
ratio1= abs(RG1y-F)/F
refRT1x= mu*F
ratio2= abs(RT1x+refRT1x)/refRT1x
ratio3= abs(RT1y-F)/F
testOK= (ratio0<1e-10) and (ratio1<1e-5) and (ratio2<1e-5) and  (ratio3<1e-5)

'''
print('RG1x= ', RG1x)
print('ratio0= ', ratio0)
print('RG1y= ', RG1y)
print('ratio1= ', ratio1)
print('refRT1x= ', refRT1x)
print('RT1x= ', RT1x)
print('ratio2= ', ratio2)
print('RT1y= ', RT1y)
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
# # oh.displayIntForcDiag('N', defFScale= 2.0)
# # oh.displayIntForcDiag('M')
# # oh.displayDispRot('uX', defFScale= 100.0)
# oh.displayDispRot('uY', defFScale= 100.0)
# # # oh.displayDispRot('rotZ', defFScale= 100.0)



