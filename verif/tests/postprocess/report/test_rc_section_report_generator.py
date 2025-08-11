# -*- coding: utf-8 -*-
''' Check reinforced concrete sections report generation. The test does not
 verify the output contents, only that the code runs and the file is
 created. The accuracy of the results is tested elsewhere.'''

import os
import math
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from postprocess import element_section_map
from postprocess.config import default_config
from postprocess.reports import rc_section_reports as sr

# Define materials.
foundationConcrete= EHE_materials.HA35
reinfSteel= EHE_materials.B500S

# Define some reinforced concrete sections.
pileDiameter= 1.25
pileCover= 0.07 # concrete cover for the piles.
stirrupsDiameterLight= 12e-3 # diameter of the stirrups
stirrupsDiameterMedium= 16e-3 # diameter of the stirrups
stirrupsDiameterHeavy= 20e-3 # diameter of the stirrups
stirrupsSpacing= 0.15 # spacing of stirrups
### Light duty piles.
pileRCSectionLightA= def_column_RC_section.RCCircularSection(name= 'pileRCSectionLightA', sectionDescr= 'pile RC section light', concrType= foundationConcrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
#### Pile main reinforcement.
pileRCSectionLightA.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 20e-3, nRebars= 24, width= math.pi*(pileDiameter-2*(pileCover+stirrupsDiameterLight)), nominalCover= pileCover+stirrupsDiameterLight)]) # in circular sections 'width' is the perimeter of the circumference surrounds the reinforcing bars
#### Pile shear reinforcement.
pileRCSectionLightA.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh", nShReinfBranches= 2, areaShReinfBranch= math.pi*(stirrupsDiameterLight)**2/4., shReinfSpacing= stirrupsSpacing, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)
#### Section distribution.
pilesRCSectsLightA= element_section_map.RCMemberSection(name='pilesRCSectsLightA', templateSections=[pileRCSectionLightA, pileRCSectionLightA])

### Shear reinforced light duty piles.
pileRCSectionLightB= def_column_RC_section.RCCircularSection(name= 'pileRCSectionLightB', sectionDescr= 'pile RC section light B', concrType= foundationConcrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
#### Pile main reinforcement.
pileRCSectionLightB.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 20e-3, nRebars= 24, width= math.pi*(pileDiameter-2*(pileCover+stirrupsDiameterLight)), nominalCover= pileCover+stirrupsDiameterLight)]) # in circular sections 'width' is the perimeter of the circumference surrounds the reinforcing bars
#### Pile shear reinforcement.
pileRCSectionLightB.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh", nShReinfBranches= 2, areaShReinfBranch= math.pi*(stirrupsDiameterMedium)**2/4., shReinfSpacing= stirrupsSpacing, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)
#### Section distribution.
pilesRCSectsLightB= element_section_map.RCMemberSection(name='pilesRCSectsLightB', templateSections=[pileRCSectionLightB, pileRCSectionLightB])

### Medium duty piles.
pileRCSectionMedium= def_column_RC_section.RCCircularSection(name= 'pileRCSectionMedium', sectionDescr= 'pile RC section medium', concrType= foundationConcrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
#### Pile main reinforcement.
firstRowCover= pileCover+stirrupsDiameterHeavy
secondRowCover= firstRowCover+pileCover
pileRCSectionMedium.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 32e-3, nRebars= 24, width= math.pi*(pileDiameter-2*firstRowCover), nominalCover= firstRowCover), def_simple_RC_section.ReinfRow(rebarsDiam= 25e-3, nRebars= 24, width= math.pi*(pileDiameter-2*secondRowCover), nominalCover= secondRowCover)])# in circular sections 'width' is the perimeter of the circumference surrounds the reinforcing bars
#### Pile shear reinforcement.
pileRCSectionMedium.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= 2, areaShReinfBranch= math.pi*(stirrupsDiameterHeavy)**2/4., shReinfSpacing= stirrupsSpacing/2, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)
#### Section distribution.
pilesRCSectsMedium= element_section_map.RCMemberSection(name='pilesRCSectsMedium', templateSections=[pileRCSectionMedium, pileRCSectionMedium])

### Heavy duty piles.
pileRCSectionHeavy= def_column_RC_section.RCCircularSection(name= 'pileRCSectionHeavy', sectionDescr= 'pile RC section heavy', concrType= foundationConcrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
#### Pile main reinforcement.
firstRowCover= pileCover+stirrupsDiameterHeavy
secondRowCover= firstRowCover+pileCover
pileRCSectionHeavy.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 32e-3, nRebars= 45, width= math.pi*(pileDiameter-2*firstRowCover), nominalCover= firstRowCover)]) # in circular sections 'width' is the perimeter of the circumference surrounds the reinforcing bars
#### Pile shear reinforcement.
pileRCSectionHeavy.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh", nShReinfBranches= 2, areaShReinfBranch= math.pi*(stirrupsDiameterHeavy)**2/4., shReinfSpacing= stirrupsSpacing/2, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)
#### Section distribution.
pilesRCSectsHeavy= element_section_map.RCMemberSection(name='pilesRCSectsHeavy', templateSections=[pileRCSectionHeavy, pileRCSectionHeavy])

pileSections= [pilesRCSectsHeavy, pilesRCSectsMedium, pilesRCSectsLightA, pilesRCSectsLightB]

# Create report generator.
cfg= default_config.get_temporary_env_config()
reportGenerator= sr.RCSectionReportGenerator(cfg= cfg)

# Create a dummy FE problem.
dummyFEProblem= xc.FEProblem()
dummyPreprocessor=  dummyFEProblem.getPreprocessor

pdfFile= reportGenerator.pdfReport(preprocessor= dummyPreprocessor, rcSectionsToReport= pileSections, writeSection2= False, showPDF= False, keepPDF= True)

# Check that file exists
testOK= os.path.isfile(pdfFile)
if os.path.exists(pdfFile):
    os.remove(pdfFile)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
