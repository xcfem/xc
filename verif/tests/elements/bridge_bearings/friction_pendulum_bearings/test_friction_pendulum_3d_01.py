# -*- coding: utf-8 -*-
''' Dummy SingleFPBearing2d regression test.'''
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

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodeHandler)

# 2. Define nodes (zero-height bearing at coordinates x=0, y=0)
nod1= modelSpace.newNode(0,0,0)
nod2= modelSpace.newNode(0,0,0)

# 3. Define axial and rotational uniaxial materials
# Material for axial direction (axialMat), high stiffness in compression,
# no tension
axialMat= typical_materials.defElastNoTensMaterial(preprocessor, name= "axialMat", E= 20000.0)
# Material for rotational/other restrained direction (tag 2)
rotMatX= typical_materials.defElasticMaterial(preprocessor, name= "rotMatX", E= 1e10)
rotMatY= typical_materials.defElasticMaterial(preprocessor, name= "rotMatY", E= 2e10)
rotMatZ= typical_materials.defElasticMaterial(preprocessor, name= "rotMatZ", E= 3e10)

# 4. Define friction model (coefficient of friction = 0.05)
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= "frictionModel", mu= .05)

# 5. Construct SingleFPBearing element (nodes 1->2, 
# combining matTags for axial and rotation, shear via friction model)
kInit= 250.0
Reff= 34.68 # Effective radius of concave sliding dish.
mass= 1.0 # Mass of the bearing.
frictionPendulumBearing= fb.def_single_friction_pendulum_bearing_3d(modelSpace, n1= nod1, n2= nod2, frictionModel= frictionModel, vertResp= axialMat, rotRespX= rotMatX, rotRespY= rotMatY, rotRespZ= rotMatZ, kInit= kInit, rEff= Reff, x= xc.Vector([0,1,0]), y= xc.Vector([-1,0,0]), shearDistI= 0.0, addRayleigh= 0, mass= mass)

# 6. Check values.
localXDir= frictionPendulumBearing.getLocalXDirection()
localYDir= frictionPendulumBearing.getLocalYDirection()
frictionModelsNames= [ fm.name for fm in frictionPendulumBearing.frictionModels]
materialsNames= [ mat.name for mat in frictionPendulumBearing.materials]
shearDistI= frictionPendulumBearing.getShearDistanceFromNodeIAsFractionOfLength()
addRayleigh= frictionPendulumBearing.getAddRayleighDampingFlag()
p0Flag= frictionPendulumBearing.getElementIsOnP0Flag()
kFactUplift= frictionPendulumBearing.getStiffnessFactorWhenUplift()
storedMass= frictionPendulumBearing.getBearingElementMass()
maxIter= frictionPendulumBearing.getMaxIter()
tol= frictionPendulumBearing.getTol()
length= frictionPendulumBearing.getLength()

dirError= math.sqrt((localXDir-xc.Vector([0,1,0])).Norm2()+(localYDir-xc.Vector([-1,0,0])).Norm2())
testOK= (dirError<1e-12)
testOK&= (storedMass==mass)
testOK&= (maxIter==25) and (tol==1e-12) and (length==0.0)
testOK&= (frictionModelsNames==[frictionModel.name])
testOK&= (materialsNames==[axialMat.name, rotMatX.name, rotMatY.name, rotMatZ.name])
testOK&= (frictionPendulumBearing.getInitialStiffnessOfHystereticComponent()==kInit)
testOK&= (frictionPendulumBearing.getConcaveSlidingDishEffRadius()==Reff)
testOK&= (shearDistI==0.0) and (addRayleigh==0)
testOK&= (p0Flag==False)
testOK&= (kFactUplift==1e-6)

'''
print('dir. error: ', dirError)
print(storedMass, mass)
print('maxIter= ', maxIter)
print('tol= ', tol)
print('length= ', length)
print(frictionModelsNames)
print(materialsNames)
print(shearDistI)
print(addRayleigh)
print(p0Flag)
print(kFactUplift)
print('test OK: ', testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
