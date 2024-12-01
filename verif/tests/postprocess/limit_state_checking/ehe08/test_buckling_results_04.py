# -*- coding: utf-8 -*-
''' Verify computation of buckling limit state parameters using a linear
    buckling analysis. Circular RC section.

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

# Reinforced concrete sections.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Circular section geometry.
diameter= 1.25 # Section diameter expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
rcCircularSection= def_column_RC_section.RCCircularSection(name='rcCircularSection', sectionDescr= 'circular column section',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
### Circular section reinforcement.
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 24
shearRebarsDiam= 16e-3
mechCover= cover-shearRebarsDiam-rebarDiam/2.0
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*mechCover), nominalCover= cover+shearRebarsDiam)])
### Put the reinforcement in the section.
rcCircularSection.mainReinf= mainReinf
## Rectangular section geometry.
sectionWidth= 1.4
sectionDepth= 0.45
rcFlatSection= def_simple_RC_section.RCRectangularSection(name= 'rcFlatSection', sectionDescr= 'flat column section', concrType= concr, reinfSteelType= steel, width= sectionWidth, depth= sectionDepth, swapReinforcementAxes= True)
### Flat section reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= rcFlatSection.defineMainReinforcement(nominalCover= 0.04, fiStirr= 12e-3, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
rcFlatSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= 12e-3, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= 12e-3, spacingZ= 0.15)

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
H= 11# 20.36 # Buckling length.
### Circular column (short column to make sure this column doesn't affect the first buckling modes).
p2= modelSpace.newKPoint(0,0,H/10.0)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6
### Flat column.
p4= modelSpace.newKPoint(5,0,H) 
p3= modelSpace.newKPoint(5,0,0)
l2= modelSpace.newLine(p3, p4)
l2.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([1,0,0]))
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
### XC circular section.
xcCircularSection= rcCircularSection.defElasticShearSection3d(preprocessor)
#### Properties to compute element buckling parameters.
circularSectionBucklingProperties= EHE_limit_state_checking.SectionBucklingProperties(
    reinforcementFactorZ= 2, # Circular section table 43.5.1 of EHE-08.
    sectionDepthZ= diameter,
    reinforcementFactorY= 2, # Circular section table 43.5.1 of EHE-08.
    sectionDepthY= diameter,
    Cz= 0.2, # clause 43.1.2 of EHE-08.
    Cy= 0.2, # clause 43.1.2 of EHE-08.
    sectionObject= rcCircularSection)
xcCircularSection.setProp('sectionBucklingProperties', circularSectionBucklingProperties)
## Finite element material
xcFlatSection= rcFlatSection.defElasticShearSection3d(preprocessor)
### Properties to compute element buckling parameters.
flatSectionBucklingProperties= EHE_limit_state_checking.SectionBucklingProperties(
    reinforcementFactorZ= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthZ= sectionWidth,
    reinforcementFactorY= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthY= sectionDepth,
    Cz= 0.24, # clause 43.1.2 of EHE-08.
    Cy= 0.24, # clause 43.1.2 of EHE-08.
    sectionObject= rcFlatSection)
xcFlatSection.setProp('sectionBucklingProperties', flatSectionBucklingProperties)

## Finite element type.
modelSpace.setDefaultMaterial(xcCircularSection)
seedElement= modelSpace.newSeedElement("ElasticBeam3d")
## Mesh generation.
### Circular column mesh generation.
l1.genMesh(xc.meshDir.I)
### Flat column mesh generation.
modelSpace.setDefaultMaterial(xcFlatSection)
l2.genMesh(xc.meshDir.I)

circSectionElements= modelSpace.defSet('circSectionElements')
for e in l1.elements:
    circSectionElements.elements.append(e)
circSectionElements.fillDownwards()
flatSectionElements= modelSpace.defSet('flatSectionElements')
for e in l2.elements:
    flatSectionElements.elements.append(e)
flatSectionElements.fillDownwards()

## Constraints.
## Circular column.
circularColumnBottomNode= p1.getNode()
circularColumnTopNode= p2.getNode()
modelSpace.fixNode('000_FF0',circularColumnBottomNode.tag)
modelSpace.fixNode('00F_FFF',circularColumnTopNode.tag)

## Flat column.
flatColumnBottomNode= p3.getNode()
flatColumnTopNode= p4.getNode()
modelSpace.fixNode('000_FF0',flatColumnBottomNode.tag)
modelSpace.fixNode('00F_FFF',flatColumnTopNode.tag)

## Loads
### Permanent load.
g1= modelSpace.newLoadPattern(name= 'G1')
N= -2000e3
MzTop= 1000e3
MzBottom= 1000e3
g1.newNodalLoad(circularColumnTopNode.tag, xc.Vector([0, 0, N, MzTop, 0, 0]))
g1.newNodalLoad(circularColumnBottomNode.tag, xc.Vector([0,0,0, MzBottom, 0, 0]))
g1.newNodalLoad(flatColumnTopNode.tag, xc.Vector([0, 0, N, MzTop/2, 0, 0]))
g1.newNodalLoad(flatColumnBottomNode.tag, xc.Vector([0,0,0, MzBottom/2, 0, 0]))

### Variable load.
q1= modelSpace.newLoadPattern(name= 'Q1')
N= -990e3
MzTop= 1000e3
MzBottom= 970e3
q1.newNodalLoad(circularColumnTopNode.tag, xc.Vector([0, 0, N, MzTop, 0, 0]))
q1.newNodalLoad(circularColumnBottomNode.tag, xc.Vector([0,0,0, MzBottom, 0, 0]))
q1.newNodalLoad(flatColumnTopNode.tag, xc.Vector([0, 0, N, MzTop/2, 0, 0]))
q1.newNodalLoad(flatColumnBottomNode.tag, xc.Vector([0,0,0, 0.4*MzBottom, 0, 0]))

## Load combinations
combContainer= combs.CombContainer()
### ULS combination.
combContainer.ULS.perm.add('combULS01','1.0*G1+1.0*Q1')


# Set environment for limit state data storage.
cfg= default_config.get_temporary_env_config()

### Analyze combinations and save buckling parameters
xcTotalSet= modelSpace.getTotalSet()
calcSet= xcTotalSet
bucklingParametersLSD= EHE_limit_state_checking.BucklingParametersLimitStateData(numModes= 4, alpha_cr_threshold= 20.0) # alpha_cr_threshold= 20.0 is clearly excessive
bucklingParametersLSD.setEnvConfig(cfg)
bucklingParametersLSD.analyzeLoadCombinations(combContainer= combContainer, setCalc= calcSet)

# Define spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition # sectContainer container
## Store the section in the data structure that the "phantom model" uses
circularColumnRCSects= element_section_map.RCMemberSection('circular', [rcCircularSection, rcCircularSection])
sectContainer.append(circularColumnRCSects)
reinfConcreteSectionDistribution.assign(elemSet= circSectionElements.elements, setRCSects= circularColumnRCSects)
flatColumnRCSects= element_section_map.RCMemberSection('flat', [rcFlatSection, rcFlatSection])
sectContainer.append(flatColumnRCSects)
reinfConcreteSectionDistribution.assign(elemSet= flatSectionElements.elements, setRCSects= flatColumnRCSects)
## Build controller.
controller= bucklingParametersLSD.getController()

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
ratio1= abs(meanCFs[0]-0.27097922513696665)/0.27097922513696665
ratio2= abs(meanCFs[1]-0.27354596382746055)/0.27354596382746055

'''
print(meanCFs[0], ratio1)
print(meanCFs[1], ratio2)
'''

import os
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
# arguments= ['My', 'Mz', 'CF', 'Leff', 'mechLambda', 'efY', 'efZ', 'mode'] 
# for arg in arguments:
#     oh.displayBeamResult(attributeName= bucklingParametersLSD.label, itemToDisp= arg, setToDisplay= xcTotalSet, beamSetDispRes= xcTotalSet, fileName=None, defFScale=0.0)
# # #########################################################

cfg.cleandirs()  # Clean after yourself.
