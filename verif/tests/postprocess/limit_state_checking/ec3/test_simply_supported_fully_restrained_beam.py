# -*- coding: utf-8 -*-
''' Beam with full lateral restraint.
   Example 01 from:

   P387: Steel Building Design: Worked Examples for Students

   SCI
   Silwood Park, Ascot, Berks SL5 7QN
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
import geom
import xc
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
from rough_calculations import ng_simple_beam as sb
from materials.sections.structural_shapes import arcelor_metric_shapes
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config

# Geometry
beamSpan= 8.0
bayWidth= 6.0

# Loads
permanentAction= 3.7e3 # N/m2
variableAction= 3.3e3 # N/m2

# Material
steel= EC3_materials.S275JR
steel.setGammaM0(1.0)
shape= EC3_materials.UBShape(steel,'UB457x191x82') # Advance UK Beam (UKB) S275
shape.sectionClass= 1

# Ultimate limit state load.
ulsLoad= 0.925*1.35*permanentAction+1.5*variableAction
ultimateDesignLoadPerMetre= ulsLoad*bayWidth


#  ___ _      _ _         ___ _                   _     __  __         _     _ 
# | __(_)_ _ (_) |_ ___  | __| |___ _ __  ___ _ _| |_  |  \/  |___  __| |___| |
# | _|| | ' \| |  _/ -_) | _|| / -_) '  \/ -_) ' \  _| | |\/| / _ \/ _` / -_) |
# |_| |_|_||_|_|\__\___| |___|_\___|_|_|_\___|_||_\__| |_|  |_\___/\__,_\___|_|
                                                                              
# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example no. 01'
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
pointHandler= preprocessor.getMultiBlockTopology.getPoints
points= list()
numLines= 10
spanIncrement= beamSpan/numLines
for i in range(0,numLines+1):
    points.append(modelSpace.newKPoint(i*spanIncrement,0.0,0.0))

## Lines
lines= list()
pA= points[0]
for p in points[1:]:
    pB= p
    newLine= modelSpace.newLine(pA,pB)
    newLine.nDiv= 1
    lines.append(newLine)
    pA= pB


# Mesh
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf3d('lin')
lin.xzVector= xc.Vector([0,1,0])
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))

xcTotalSet= modelSpace.getTotalSet()
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints (simply supported beam)
modelSpace.fixNode('000_0FF',points[0].getNode().tag)
modelSpace.fixNode('F00_FFF',points[-1].getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['permanentAction','variableAction']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Permanent action.
deadLoad= xc.Vector([0.0,0.0,-permanentAction*bayWidth])
cLC= loadCaseManager.setCurrentLoadCase('permanentAction')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(deadLoad)
    
## Variable action.
liveLoad= xc.Vector([0.0,0.0,-variableAction*bayWidth])
cLC= loadCaseManager.setCurrentLoadCase('variableAction')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(liveLoad)
    
## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.24875*permanentAction+1.5*variableAction')
### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

# Compute internal forces.

## Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg

## Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

## Limit states to calculate internal forces for.
limitStates= [lsd.steelNormalStressesResistance, # Normal stresses resistance.
lsd.steelShearResistance, # Shear stresses resistance
]

## Create EC3 Member objects.
ec3CalcSet= modelSpace.defSet('ec3CalcSet') # Elements to be checked as EC3 members.
ec3Members= list() # EC3 members.
for l in xcTotalSet.getLines:
    member= EC3_limit_state_checking.Member(name= l.name, ec3Shape= shape, lstLines= [l])
    #member.setControlPoints()
    member.installULSControlRecorder(recorderType="element_prop_recorder", calcSet= ec3CalcSet)
    ec3Members.append(member)
## Compute internal forces for each combination
for ls in limitStates:
    ls.analyzeLoadCombinations(combContainer,ec3CalcSet, bucklingMembers= ec3Members)

## Check normal stresses.
### Limit state to check.
limitState= lsd.steelNormalStressesResistance
### Build controller.
controller= EC3_limit_state_checking.BiaxialBendingNormalStressController(limitStateLabel= limitState.label)
### Perform checking.
bendingAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

### Get the maximum efficiency.
maxBendingCF= 0.0
for e in xcTotalSet.elements:
    CF1= e.getProp('ULS_normalStressesResistanceSect1').CF
    CF2= e.getProp('ULS_normalStressesResistanceSect2').CF
    maxBendingCF= max(maxBendingCF, CF1, CF2)

## Check shear.
### Limit state to check.
limitState= lsd.steelShearResistance
### Build controller.
controller= EC3_limit_state_checking.ShearController(limitState.label)
### Perform checking.
shearAverage= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

### Get the maximum efficiency.
maxShearCF= 0.0
for e in xcTotalSet.elements:
    CF1= e.getProp('ULS_shearResistanceSect1').CF
    CF2= e.getProp('ULS_shearResistanceSect2').CF
    maxShearCF= max(maxShearCF, CF1, CF2)

# Check results.
## Average values.
ratio= ((bendingAverage[0]-0.6024629441558013)/0.6024629441558013)**2
ratio+= ((bendingAverage[1]-0.6024629441558034)/0.6024629441558034)**2
ratio+= ((shearAverage[0]-0.15206288931414974)/0.15206288931414974)**2
ratio+= ((shearAverage[1]-0.15206288931414977)/0.15206288931414977)**2
ratio= math.sqrt(ratio)

# Value in the text maxBendingCF= 0.91 (See 6.2.5(2))
ratio2= abs(maxBendingCF-0.9128226426603291)/0.912822642660329
# Value in the text maxShearCF= 0.3 (See 6.2.6(2))
ratio3= abs(maxShearCF-0.3041257786282982)/0.3041257786282982

#   ___       _             _   
#  / _ \ _  _| |_ _ __ _  _| |_ 
# | (_) | || |  _| '_ \ || |  _|
# \___/ \_,_|\__| .__/\_,_|\__|
#                |_|            
'''
print('Ultimate design load per meter Fd= ', ultimateDesignLoadPerMetre/1e3, 'kN/m')
print('bending average= ', bendingAverage)
print('maximum shear efficiency: ', maxShearCF)
print('shear average= ', shearAverage)
print('ratio= ', ratio)
'''

cfg.cleandirs() # Clean after yourself.
from misc_utils import log_messages as lmsg
import os
fname= os.path.basename(__file__)
if(ratio<1e-8 and ratio2<1e-8 and ratio3<1e-8):
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
# oh.displayBeamResult(attributeName=lsd.normalStressesResistance.label, itemToDisp='CF', beamSetDispRes=ec3CalcSet, setToDisplay=xcTotalSet)
# # Display shear efficiency.
# oh.displayBeamResult(attributeName=lsd.shearResistance.label, itemToDisp='CF', beamSetDispRes=ec3CalcSet, setToDisplay=xcTotalSet)
