# -*- coding: utf-8 -*-
''' Lateral torsional buckling on cantilever beam. Plane model. Test based on 
    the example at section 4.3 of the article:

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

import math
import geom
import xc
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config

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
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

#Materials
## Profile geometry
xcSection= shape.defElasticShearSection2d(preprocessor)

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
lin= trfs.newLinearCrdTransf2d('lin')
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(xcSection)
modelSpace.newSeedElement("ElasticBeam2d")

xcTotalSet= modelSpace.getTotalSet()
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints (simply supported beam)
modelSpace.fixNode('000', p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['permanentAction','variableAction']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Permanent action.
deadLoad= xc.Vector([0.0,-permanentAction])
cLC= loadCaseManager.setCurrentLoadCase('permanentAction')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(deadLoad)

## Variable action.
liveLoad= xc.Vector([0.0,-variableAction])
cLC= loadCaseManager.setCurrentLoadCase('variableAction')
for e in xcTotalSet.elements:
    e.vector2dUniformLoadGlobal(liveLoad)

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

# Create EC3 members.
ec3Members= list() # EC3 members.
for l in xcTotalSet.getLines:
    member= EC3_limit_state_checking.Member(name= l.name, ec3Shape= shape, lstLines= [l], beamSupportCoefs= beamSupportCoefs)
    #member.setControlPoints()
    ec3Members.append(member)
    
## Populate the ec3CalcSet set.     
ec3CalcSet= modelSpace.defSet('ec3CalcSet')
for member in ec3Members:
    member.installULSControlRecorder(recorderType="element_prop_recorder", calcSet= ec3CalcSet)
ec3CalcSet.fillDownwards()

## Compute internal forces for each combination
for ls in limitStates:
    ls.analyzeLoadCombinations(combContainer= combContainer, setCalc= ec3CalcSet, bucklingMembers= ec3Members)

## Check normal stresses.
### Limit state to check.
limitState= limitStates[0]
### Build controller.
controller= limitState.getController(biaxialBending= False)
### Perform checking.
bendingAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

### Get the lateral torsional buckling reduction factor.
chiLT= 0.0
for e in xcTotalSet.elements:
    chiLT+= e.getProp('chiLT')

chiLT/= len(xcTotalSet.elements)

### Get the maximum efficiency.
maxBendingCF= 0.0
for e in xcTotalSet.elements:
    CF1= e.getProp('ULS_normalStressesResistanceSect1').CF
    CF2= e.getProp('ULS_normalStressesResistanceSect2').CF
    maxBendingCF= max(maxBendingCF, CF1, CF2)

## Check shear.
### Limit state to check.
limitState= limitStates[1]
### Build controller.
controller= limitState.getController()
### Perform checking.
shearAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

### Get the maximum efficiency.
maxShearCF= 0.0
for e in xcTotalSet.elements:
    CF1= e.getProp('ULS_shearResistanceSect1').CF
    CF2= e.getProp('ULS_shearResistanceSect2').CF
    maxShearCF= max(maxShearCF, CF1, CF2)

# Check results.
## Lateral torsional buckling reduction factor.
## The value of the lateral torsional buckling reduction factor obtained in
## the article is 0.672 greater than the value obtained here (0.661), so
## the result is on the safety side.
ratio0= abs(chiLT-0.6609805363859881)/0.6609805363859881
## Maximum bending capacity factor.
ratio1= abs(maxBendingCF-1.783045394470495)/1.783045394470495
## Maximum shear capacity factor.
ratio2= abs(maxShearCF-0.12044200463987757)/0.12044200463987757

'''
print('Lateral torsional buckling reduction factor: ', chiLT, ratio0)
print('Maximum bending capacity factor:', maxBendingCF, ratio1)
print('Maximum shear capacity factor:', maxShearCF, ratio2)
'''

cfg.cleandirs() # Clean after yourself.
from misc_utils import log_messages as lmsg
import os
fname= os.path.basename(__file__)
if(ratio0<1e-8 and ratio1<1e-8 and ratio2<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # Display lateral buckling reduction factor.
# oh.displayElementValueDiagram('chiLT', setToDisplay= ec3CalcSet)

# # Display normal stresses efficiency.
# oh.displayBeamResult(attributeName= limitStates[0].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet)
# # Display shear efficiency.
# oh.displayBeamResult(attributeName= limitStates[1].label, itemToDisp='CF', beamSetDispRes= ec3CalcSet, setToDisplay=xcTotalSet)
