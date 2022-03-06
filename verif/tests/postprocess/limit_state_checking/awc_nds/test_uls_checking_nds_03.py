# -*- coding: utf-8 -*-
''' Simply supported beam-column capacity check example E1.7
    taken from the document NDS Structural Wood Design
    Examples 2015/2018 Edition.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import xc_base
import geom
import xc
from materials.awc_nds import structural_panels
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from postprocess.config import default_config
from postprocess import limit_state_data as lsd
from materials.awc_nds import AWCNDS_limit_state_checking as nds
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import dimensional_lumber
from materials.awc_nds import AWCNDS_analysis_context

# Loads
from actions import load_cases as lcm

# Units
inchToMeter= 2.54/100.0
footToMeter= 0.3048
poundToN= 4.44822
psiToPa= 6894.76
psfToPa= 47.88026

def getLoadCombDurationFactor(loadComb):
    dl= ('deadLoad' in loadComb)
    sl= ('snowLoad' in loadComb)
    wl= ('windLoad' in loadComb)
    return AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, snowLoad= sl, windLoad= wl)

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example 4.7 Structural Wood Design'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials
wood= dimensional_lumber.SouthernPineWood(name='SouthernPine', grade= 'no_1', sub_grade= '')
columnSection= AWCNDS_materials.DimensionLumberSection(name= '2x6', woodMaterial= wood)
xcSection= columnSection.defElasticShearSection3d(preprocessor)

# Model geometry
height= 9*footToMeter
w_trib= 4.0*footToMeter

## Points.
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPoint(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPoint(geom.Pos3d(0.0,0.0,height))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 10

# Mesh generation.
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

# Constraints
n0= p0.getNode()
n1= p1.getNode()
modelSpace.fixNode('000_FF0',n0.tag)
modelSpace.fixNode('00F_FFF',n1.tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','snowLoad','windLoad','windLoadNeg']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
deadLoadVector= xc.Vector([0.0,0.0,-560*poundToN,0,0,0])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
cLC.newNodalLoad(n1.tag,deadLoadVector)

## Snow load.
snowLoadVector= xc.Vector([0.0,0.0,-840*poundToN,0,0,0])
cLC= loadCaseManager.setCurrentLoadCase('snowLoad')
cLC.newNodalLoad(n1.tag,snowLoadVector)

## Wind load.
windLoadVector= xc.Vector([-25*psfToPa*w_trib,0.0,0.0])
cLC= loadCaseManager.setCurrentLoadCase('windLoad')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(windLoadVector)
    
## Wind load.
cLC= loadCaseManager.setCurrentLoadCase('windLoadNeg')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(-windLoadVector)
    
## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state.
combContainer.ULS.perm.add('LC1','1.0*deadLoad+1.0*snowLoad+1.0*windLoad')
combContainer.ULS.perm.add('LC2','1.0*deadLoad+1.0*snowLoad')
combContainer.ULS.perm.add('LC3','1.0*deadLoad')
combContainer.ULS.perm.add('LC4','1.0*deadLoad+1.0*snowLoad+1.0*windLoadNeg')

# Compute internal forces.

## Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg

## Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

## Limit states to calculate internal forces for.
limitStates= [lsd.normalStressesResistance, # Normal stresses resistance.
#lsd.shearResistance, 
]

## Create NDS Member objects.
ndsCalcSet= modelSpace.defSet('ndsCalcSet') # Elements to be checked as NDS members.
ndsMembers= list() # NDS members list.
for l in xcTotalSet.getLines:
    Lx= .01*l.getLength() # continuously braced.
    member= nds.Member(name= l.name, section= columnSection, unbracedLengthX= Lx, unbracedLengthZ= l.getLength(), lstLines= [l], memberRestraint= AWCNDS_materials.MemberRestraint.compressionEdgeSupport, loadCombDurationFactorFunction= getLoadCombDurationFactor)
    #member.setControlPoints()
    member.installULSControlRecorder(recorderType="element_prop_recorder", calcSet= ndsCalcSet)
    ndsMembers.append(member)

analysisContext= AWCNDS_analysis_context.SimpleAnalysisContext(modelSpace= modelSpace, calcSet= ndsCalcSet, bucklingMembers= ndsMembers, silent= True)

## Compute internal forces for each combination
for ls in limitStates:
    loadCombinations= preprocessor.getLoadHandler.getLoadCombinations
    #Putting combinations inside XC.
    loadCombinations= ls.dumpCombinations(combContainer,loadCombinations)
    analysisContext.calcInternalForces(loadCombinations, ls)
    
outCfg= lsd.VerifOutVars(setCalc=ndsCalcSet, appendToResFile='Y', listFile='N', calcMeanCF='Y')
limitState= lsd.normalStressesResistance
outCfg.controller= nds.BiaxialBendingNormalStressController(limitState.label)
average= limitState.runChecking(outCfg)

ratio= ((average[0]-0.6040967008241481)/0.6040967008241481)**2
ratio+= ((average[1]-0.604096700824146)/0.604096700824146)**2
ratio0= math.sqrt(ratio)

# Label to get the property that contains the control vars.
label1= outCfg.controller.limitStateLabel+outCfg.controller.getSectionLabel(0)
label2= outCfg.controller.limitStateLabel+outCfg.controller.getSectionLabel(1)
maxCF= 0.0
# Reference value obtained from verification test: test_bending_and_axial_compression.py
refMaxCF= 0.887544474580459
for e in ndsCalcSet.elements:
    CF= e.getProp(label1).CF
    maxCF= max(maxCF, CF)
    CF= e.getProp(label2).CF
    maxCF= max(maxCF, CF)

ratio1= abs(maxCF-refMaxCF)/refMaxCF

'''
print('average= ', average)
print('ratio0= ', ratio0)
print('max. CF:', maxCF)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-10) and (ratio1<1e-7)):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")

# #########################################################
# Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayElementValueDiagram('chiLT', setToDisplay= ndsCalcSet)

# oh.displayBeamResult(attributeName=lsd.normalStressesResistance.label, itemToDisp='CF', beamSetDispRes=ndsCalcSet, setToDisplay=xcTotalSet)

