# -*- coding: utf-8 -*-
''' Verify computation of buckling limit state parameters using a linear
    buckling analysis. 
    Twin columns with rectangular RC section and different orientations.
'''

from __future__ import division
from __future__ import print_function

import math
import os
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg
from actions import combinations as combs
from postprocess.config import default_config
from postprocess import RC_material_distribution
from postprocess import element_section_map

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
sectionWidth= 0.45
sectionDepth= 1.4
rcSection= def_simple_RC_section.RCRectangularSection(name= 'flatColumnsRCSection', sectionDescr= 'columns section', concrType= concr, reinfSteelType= steel, width= sectionWidth, depth= sectionDepth, swapReinforcementAxes= False)
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
H= 13.0 # Buckling length.
p2= modelSpace.newKPoint(0,0,H)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6
p4= modelSpace.newKPoint(5,0,H)
p3= modelSpace.newKPoint(5,0,0)
l2= modelSpace.newLine(p3, p4)
l2.nDiv= 6

## Geometric transformations
corot1= modelSpace.newCorotCrdTransf("corot1", xzVector= xc.Vector([1,0,0]))
corot2= modelSpace.newCorotCrdTransf("corot2", xzVector= xc.Vector([0,-1,0]))

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
modelSpace.setDefaultCoordTransf(corot1)
modelSpace.newSeedElement("ElasticBeam3d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)
modelSpace.setDefaultCoordTransf(corot2)
modelSpace.newSeedElement("ElasticBeam3d")
l2.genMesh(xc.meshDir.I)

## Constraints.
bottomNode1= p1.getNode()
topNode1= p2.getNode()
modelSpace.fixNode('000_FF0',bottomNode1.tag) # pinned
modelSpace.fixNode('00F_FFF',topNode1.tag) # pinned
bottomNode2= p3.getNode()
topNode2= p4.getNode()
modelSpace.fixNode('000_FF0',bottomNode2.tag) # pinned
modelSpace.fixNode('00F_FFF',topNode2.tag) # pinned

## Loads
### Permanent load.
g1= modelSpace.newLoadPattern(name= 'G1')
Ng= -5000e3
g1.newNodalLoad(topNode1.tag, xc.Vector([0, 0, Ng, 0, 0, 0]))
g1.newNodalLoad(topNode2.tag, xc.Vector([0, 0, Ng, 0, 0, 0]))
### Variable load.
q1= modelSpace.newLoadPattern(name= 'Q1')
Nq= Ng
q1.newNodalLoad(topNode1.tag, xc.Vector([0, 0, Nq, 0, 0, 0]))
q1.newNodalLoad(topNode2.tag, xc.Vector([0, 0, Nq, 0, 0, 0]))


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
ratio1= abs(meanCFs[0]-0.61652927)/0.61652927
ratio2= abs(meanCFs[1]-0.61652927)/0.61652927

'''
print(meanCFs[0], ratio1)
print(meanCFs[1], ratio2)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # Load control variables to display:
# bucklingParametersLSD.readControlVars(modelSpace= modelSpace)
# # Display them.
# # arguments= ['My', 'Mz', 'CF', 'Leff', 'mechLambda', 'efY', 'efZ', 'mode'] 
# arguments= ['My'] 
# for arg in arguments:
#     oh.displayBeamResult(attributeName= bucklingParametersLSD.label, itemToDisp= arg, setToDisplay= xcTotalSet, beamSetDispRes= xcTotalSet, fileName=None, defFScale=0.0)
# # #########################################################
    
cfg.cleandirs()  # Clean after yourself.
