# -*- coding: utf-8 -*-

import math
import xc
from materials import typical_materials

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler 

soilMat= typical_materials.defPressureDependentMultiYield02Material(preprocessor= preprocessor, name= 'soilMat', nd= 2, rho= 1.8,  refShearModul= 9.6e3, refBulkModul= 2.7e4, frictionAng= math.radians(36),  peakShearStra= 0.1, refPress= -101.0, pressDependCoe= 0.0, phaseTransformAngle= math.radians(26), contractionParam1= 0.067, contractionParam3= 0.23, dilationParam1= 0.06, dilationParam3= 0.27, numberOfYieldSurf= 20, contractionParam2= 5.0, dilationParam2= 3.0, liquefactionParam1= 1.0, liquefactionParam2= 0.0, e= 0.77, volLimit1= 0.9, volLimit2= 0.02, volLimit3= 0.7, atm= 101.0)
soilMat.stage= 1

soilMaterialStageA= soilMat.stage


# create wrapper material for initial state analysis
matWrapper= typical_materials.defInitialStateAnalysisWrapper(preprocessor= preprocessor, name= 'matWrapper', ndim= 2, encapsulatedMaterial= soilMat)

encapsulatedMaterial= matWrapper.encapsulatedMaterial
encapsulatedMaterialStageA= encapsulatedMaterial.stage
ratio1= (soilMaterialStageA-1)+(encapsulatedMaterialStageA-1)

encapsulatedMaterial.updateMaterialStage(0)
# This changes the stage for ALL the materials:
ratio2= (soilMat.stage+encapsulatedMaterial.stage)

# Check initial state analysis phase set/get
phaseA= matWrapper.getInitialStateAnalysisPhase()
matWrapper.setInitialStateAnalysisPhase(True)
phaseB= matWrapper.getInitialStateAnalysisPhase()

testOK= (ratio1==0) and (ratio2==0) and (not phaseA) and phaseB

'''
print('soil material stage before: ', soilMaterialStageA)
print('encapsulated material stage before: ', encapsulatedMaterialStageA)
print('ratio1= ', ratio1)
print('soil material stage: ', soilMat.stage)
print('encapsulated material stage: ', encapsulatedMaterial.stage)
print('ratio2= ', ratio2)
print('initial state analysis phase before: ', phaseA)
print('initial state analysis phase after: ', phaseB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
