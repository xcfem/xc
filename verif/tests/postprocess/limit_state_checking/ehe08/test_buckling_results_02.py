# -*- coding: utf-8 -*-
''' Verify computation of buckling limit state parameters using a linear
    buckling analysis. Rectangular RC section.

'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from actions import combinations as combs
from postprocess.config import default_config
from postprocess import limit_state_data as lsd
from postprocess import RC_material_distribution
from postprocess import element_section_map

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
sectionWidth= 1.4
sectionDepth= 0.45
rcSection= def_simple_RC_section.RCRectangularSection(name= 'flatColumnsRCSection', sectionDescr= 'columns section', concrType= concr, reinfSteelType= steel, width= sectionWidth, depth= sectionDepth, swapReinforcementAxes= True)
## Reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= rcSection.defineMainReinforcement(nominalCover= 0.04, fiStirr= 12e-3, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
rcSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= 12e-3, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= 12e-3, spacingZ= 0.15)

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
H= 13.0#20.36 # Buckling length.
p2= modelSpace.newKPoint(0,0,H)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
xcSection= rcSection.defElasticShearSection3d(preprocessor)
### Properties to compute element buckling parameters.
sectionBucklingProperties= EHE_limit_state_checking.SectionBucklingProperties(
    reinforcementFactorZ= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthZ= sectionWidth,
    reinforcementFactorY= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthY= sectionDepth,
    Cz= 0.24, # clause 43.1.2 of EHE-08.
    Cy= 0.24, # clause 43.1.2 of EHE-08.
    sectionObject= rcSection)
xcSection.setProp('sectionBucklingProperties', sectionBucklingProperties)
modelSpace.setDefaultMaterial(xcSection)

## Finite element type.
seedElement= modelSpace.newSeedElement("ElasticBeam3d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)

## Constraints.
bottomNode= p1.getNode()
topNode= p2.getNode()
modelSpace.fixNode('000_FF0',bottomNode.tag)
modelSpace.fixNode('00F_FFF',topNode.tag)

## Loads
### Permanent load.
g1= modelSpace.newLoadPattern(name= 'G1')
N= -2000e3
MzTop= 1000e3
MzBottom= -1000e3
g1.newNodalLoad(topNode.tag, xc.Vector([0, 0, N, MzTop, 0, 0]))
g1.newNodalLoad(bottomNode.tag, xc.Vector([0,0,0, MzBottom, 0, 0]))
### Variable load.
q1= modelSpace.newLoadPattern(name= 'Q1')
N= -990e3
MzTop= 500e3
MzBottom= 370e3
q1.newNodalLoad(topNode.tag, xc.Vector([0, 0, N, MzTop, 0, 0]))
q1.newNodalLoad(bottomNode.tag, xc.Vector([0,0,0, MzBottom, 0, 0]))

## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.ULS.perm.add('combULS01','1.0*G1+1.0*Q1')


# Set environment for limit state data storage.
cfg= default_config.get_temporary_env_config()

### Analyze combinations and save buckling parameters
xcTotalSet= modelSpace.getTotalSet()
calcSet= xcTotalSet
bucklingParametersLSD= EHE_limit_state_checking.BucklingParametersLimitStateData(numModes= 4)
bucklingParametersLSD.setEnvConfig(cfg)
bucklingParametersLSD.analyzeLoadCombinations(combContainer= combContainer, setCalc= calcSet)

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition # sectContainer container
## Store the section in the data structure that the "phantom model" uses
columnRCSects= element_section_map.RCMemberSection('test', [rcSection, rcSection])
sectContainer.append(columnRCSects)
reinfConcreteSectionDistribution.assign(elemSet= calcSet.elements, setRCSects= columnRCSects)
## Build controller.
controller= EHE_limit_state_checking.BiaxialBucklingController(bucklingParametersLSD.label)

# And now check normal stresses.
## variables that control the output of the checking:
### setCalc: set of elements to be checked.
### crossSections: cross sections for each element.
### controller: object that controls the limit state checking.
### appendToResFile:  'Yes','Y','y',.., if results are appended to 
###                   existing file of results (defaults to 'N')
### listFile: 'Yes','Y','y',.., if latex listing file of results 
###           is desired to be generated (defaults to 'N')
### calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
###               meant to be calculated (defaults to 'N')
### threeDim: true if it's 3D (Fx,Fy,Fz,Mx,My,Mz) 
###           false if it's 2D (Fx,Fy,Mz).
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
meanCFs= bucklingParametersLSD.check(setCalc= calcSet, crossSections= reinfConcreteSectionDistribution,appendToResFile='N',listFile='N',calcMeanCF='Y', controller= controller, threeDim= True)
feProblem.errFileName= "cerr" # From now on display errors if any.

# Check results. The reference values doesn't come from a benchmark test,
# they serve only to verify that the code run as intended.
ratio1= abs(meanCFs[0]-0.9213895059244708)/0.9213895059244708
ratio2= abs(meanCFs[1]-0.9852785840915605)/0.9852785840915605

# print(meanCFs)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayFEMesh()
# # Load control variables to display:
# bucklingParametersLSD.readControlVars(modelSpace= modelSpace)
# arguments= ['My', 'Mz', 'CF', 'Leff', 'mechLambda', 'efY', 'efZ', 'mode'] 
# arguments= ['Leff', 'CF', 'mechLambda']
# for arg in arguments:
#     oh.displayBeamResult(attributeName= bucklingParametersLSD.label, itemToDisp= arg, setToDisplay= xcTotalSet, beamSetDispRes= xcTotalSet, fileName=None, defFScale=0.0)
    
cfg.cleandirs()  # Clean after yourself.
