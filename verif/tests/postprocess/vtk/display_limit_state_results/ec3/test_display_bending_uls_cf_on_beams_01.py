# -*- coding: utf-8 -*-
''' Lateral torsional buckling on cantilever beam. Test based on the example 
   at section 4.3 of the article:

    "Stability Study of Cantilever-Beams – Numerical Analysis and Analytical 
     Calculation (LTB)" Matthias Kraus, Nicolae-Andrei Crișan, Björn Witto

    URL: https://d-nb.info/125069664X/34
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import os
import xc
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from postprocess import output_handler

# Geometry
beamSpan= 4.0

# Loads
qz= 6e3 # Uniform load.
permanentAction= 0.4*qz
variableAction= qz-permanentAction

# Material
steel= EC3_materials.S355JR
gammaM0= 1.00
steel.gammaM= gammaM0 
shape= EC3_materials.IPEShape(steel,"IPE_200")
shape.sectionClass= 1

#  ___ _      _ _         ___ _                   _     __  __         _     _ 
# | __(_)_ _ (_) |_ ___  | __| |___ _ __  ___ _ _| |_  |  \/  |___  __| |___| |
# | _|| | ' \| |  _/ -_) | _|| / -_) '  \/ -_) ' \  _| | |\/| / _ \/ _` / -_) |
# |_| |_|_||_|_|\__\___| |___|_\___|_|_|_\___|_||_\__| |_|  |_\___/\__,_\___|_|
                                                                              
# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Cantilever LTB ULS check'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#Materials
## Profile geometry
xcSection= shape.defElasticShearSection3d(preprocessor)

# Model geometry
# We use a set of small lines to simulate the lateral restraint
# of the upper flange.
## Points.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(beamSpan, 0, 0)

## Lines
l= modelSpace.newLine(p1,p2)
l.nDiv= 6

# Mesh
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf3d('lin')
lin.xzVector= xc.Vector([0,1,0])
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(xcSection)
modelSpace.newSeedElement("ElasticBeam3d")

xcTotalSet= modelSpace.getTotalSet()
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints (simply supported beam)
modelSpace.fixNode('000_000', p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['permanentAction','variableAction']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Permanent action.
deadLoad= xc.Vector([0.0,0.0,-permanentAction])
cLC= loadCaseManager.setCurrentLoadCase('permanentAction')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(deadLoad)
    
## Variable action.
liveLoad= xc.Vector([0.0,0.0,-variableAction])
cLC= loadCaseManager.setCurrentLoadCase('variableAction')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(liveLoad)
    
## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.35*permanentAction+1.50*variableAction')
### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

# Compute internal forces.

## Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg

## Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

## Limit states to calculate internal forces for.
limitStates= [EC3_limit_state_checking.normalStressesResistance, # Normal stresses resistance.
EC3_limit_state_checking.shearResistance, # Shear stresses resistance
]

## Create EC3 Member objects.
### Cantilever beam support coefficients ky= 2-0 and k1= 0.5
beamSupportCoefs= EC3_limit_state_checking.BeamSupportCoefficients(ky= 2.0, kw= 1.0, k1= 0.5, k2= 1.0)
# Elements to be checked as EC3 members.
ec3CalcSet= modelSpace.defSet('ec3CalcSet') 
ec3Members= list() # EC3 members.
for l in xcTotalSet.getLines:
    member= EC3_limit_state_checking.Member(name= l.name, ec3Shape= shape, lstLines= [l], beamSupportCoefs= beamSupportCoefs)
    ec3Members.append(member)
    
## Populate the ec3CalcSet set. 
ec3CalcSet= modelSpace.defSet('ec3CalcSet') 
for member in ec3Members:
    member.installULSControlRecorder(recorderType="element_prop_recorder", calcSet= ec3CalcSet)
ec3CalcSet.fillDownwards()

## Compute internal forces for each combination
for ls in limitStates:
    ls.analyzeLoadCombinations(combContainer, ec3CalcSet, bucklingMembers= ec3Members)

## Check normal stresses.
### Limit state to check.
limitState= limitStates[0]
### Build controller.
controller= limitState.getController(biaxialBending= True)
### Perform checking.
bendingAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

## Check shear.
### Limit state to check.
limitState= limitStates[1]
### Build controller.
controller= limitState.getController()
### Perform checking.
shearAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

# Check results.
fname= os.path.basename(__file__)

oh= output_handler.OutputHandler(modelSpace)
outputFileNames= list()
# Display lateral buckling reduction factor.
# oh.displayElementValueDiagram('chiLT', setToDisplay= ec3CalcSet)
chiLTOutputFileName= '/tmp/'+fname.replace('.py', '_chi_lt.jpeg')
oh.displayElementValueDiagram('chiLT', setToDisplay= ec3CalcSet, fileName= chiLTOutputFileName)
outputFileNames.append(chiLTOutputFileName)

# Display normal stresses efficiency.
# oh.displayBeamResult(attributeName= limitStates[0].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet)
bendingCFOutputFileName= '/tmp/'+fname.replace('.py', '_bending_cf.jpeg')
oh.displayBeamResult(attributeName= limitStates[0].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet, fileName= bendingCFOutputFileName)
outputFileNames.append(bendingCFOutputFileName)

# Display shear efficiency.
# oh.displayBeamResult(attributeName= limitStates[1].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet)
shearCFOutputFileName= '/tmp/'+fname.replace('.py', '_shear_cf.jpeg')
oh.displayBeamResult(attributeName= limitStates[1].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet, fileName= shearCFOutputFileName)
outputFileNames.append(shearCFOutputFileName)

##  Check that all the tree file exists
testOK= True
for fName in outputFileNames:
    testOK= testOK and os.path.isfile(fName)

# print(testOK)

import os
from misc_utils import log_messages as lmsg
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

cfg.cleandirs() # Clean after yourself.
os.remove(chiLTOutputFileName)
os.remove(bendingCFOutputFileName)
os.remove(shearCFOutputFileName)
