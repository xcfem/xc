# -*- coding: utf-8 -*-
''' Test remove_accents function.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2024 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from model import predefined_spaces
from postprocess import RC_material_distribution
from postprocess import element_section_map
import json

fName= os.path.basename(__file__)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
outputFileName= pth+"/"+fName.replace('.py','.json')

## Define materials.
concrete= EHE_materials.HA30
steel= EHE_materials.B500S
## Shell section
shellDepth= 0.5
shellRCSection= element_section_map.RCSlabBeamSection(name= 'shellRCSection', sectionDescr= 'shell test RC section', concrType= concrete, reinfSteelType= steel, depth= 0.5)
shellRCSection.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 12e-3, rebarsSpacing= 0.15, nominalCover=0.035)])
shellRCSection.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 12e-3, rebarsSpacing= 0.15, nominalCover= 0.035)])
shellRCSection.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 20e-3, rebarsSpacing= 0.15, nominalCover=0.035+12e-3)])
shellRCSection.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 20e-3, rebarsSpacing= 0.15, nominalCover=0.035+12e-3)])
## Beam-column section.
beamColumnRCSection= def_simple_RC_section.RCRectangularSection(name= 'beamColumnRCSection', sectionDescr= 'test beam column section', concrType= concrete, reinfSteelType= steel, width= 1.45, depth= 0.45, swapReinforcementAxes= True)
#firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= beamColumnRCSection.defineMainReinforcement(nominalCover= 0.035, fiStirr= 12e-3, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
#beamColumnRCSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= 12e-3, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= 12e-3, spacingZ= 0.15)
### Section properties that will be used to compute element buckling parameters.
beamColumnBucklingProperties= EHE_limit_state_checking.SectionBucklingProperties(
    reinforcementFactorZ= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthZ= 1.45,
    reinforcementFactorY= 1, # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthY= 0.45,
    Cz= 0.24, # clause 43.1.2 of EHE-08.
    Cy= 0.24, # clause 43.1.2 of EHE-08.
    sectionObject= beamColumnRCSection)

### Section distribution.
beamColumnRCSects= element_section_map.RCMemberSection(name='beamColumnRCSects', templateSections=[beamColumnRCSection, beamColumnRCSection])

# Define FE problem.
feProblem= xc.FEProblem()
feProblem.title= 'XC JSON encoder/decoder test.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Define problem type.
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
## Define mesh geometry.
### Define corners.
pt0= modelSpace.newKPoint(0.0,0.0,0.0)
pt1= modelSpace.newKPoint(2.0,0.0,0.0)
pt2= modelSpace.newKPoint(2.0,2.0,0.0)
pt3= modelSpace.newKPoint(0.0,2.0,0.0)
pt10= modelSpace.newKPoint(0.0,0.0,4.0)
pt11= modelSpace.newKPoint(2.0,0.0,4.0)
pt12= modelSpace.newKPoint(2.0,2.0,4.0)
pt13= modelSpace.newKPoint(0.0,2.0,4.0)
### Define quad surface.
s= modelSpace.newQuadSurface(pt0, pt1, pt2, pt3)
s.nDivI= 2
s.nDivJ= 2
### Define lines 
l0= modelSpace.newLine(pt0, pt10)
l1= modelSpace.newLine(pt1, pt11)
l2= modelSpace.newLine(pt2, pt12)
l3= modelSpace.newLine(pt3, pt13)

## Define mesh
### Define shell mesh.
xcMaterial= shellRCSection.getElasticMembranePlateSection(preprocessor= preprocessor)
modelSpace.setDefaultMaterial(xcMaterial)
modelSpace.newSeedElement('ShellMITC4')
s.genMesh(xc.meshDir.I)
### Define beam-column mesh.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
xcMaterial= beamColumnRCSection.defElasticShearSection3d(preprocessor= preprocessor)
#### Set buckling properties.
xcMaterial.setProp('sectionBucklingProperties', beamColumnBucklingProperties)
modelSpace.setDefaultMaterial(xcMaterial)
modelSpace.newSeedElement('ElasticBeam3d')
for l in [l0, l1, l3, l3]:
    l.genMesh(xc.meshDir.I)

## Define FE sets.
### Shell element sets.
shellSet= modelSpace.defSet('shellSet', elements= s.elements)
### Beam-column element sets.
columnSet= modelSpace.defSet('columnSet', elements= l0.elements)
for l in [l1, l3, l3]:
    for e in l.elements:
        columnSet.elements.append(e)
columnSet.fillDownwards()

reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition #sectContainer container
sectContainer.append(beamColumnRCSects)
assigned= reinfConcreteSectionDistribution.assign(elemSet= columnSet.elements, setRCSects= beamColumnRCSects)

sectContainer.append(shellRCSection)
assigned= reinfConcreteSectionDistribution.assign(elemSet= shellSet.elements, setRCSects= shellRCSection)

reinfConcreteSectionDistribution.writeToJSON(fileName= outputFileName)

# Read the JSON file
with open(outputFileName, 'r') as f:
    item_dict= json.load(f)
    
# Compute total number of keys
def total_keys(test_dict):
    return (0 if not isinstance(test_dict, dict) 
    else len(test_dict) + sum(total_keys(val) for val in test_dict.values()))

numKeys= total_keys(item_dict)

# Check some results.
testOK= (numKeys==57)

'''
print('total number of keys: ', numKeys)
print(outputFileName)
'''


from misc_utils import log_messages as lmsg
fName= os.path.basename(__file__)
if(testOK):
    print("test "+fName+": ok.")
else:
    lmsg.error('test: '+fName+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
os.system('rm -f '+outputFileName) # Your garbage you clean it
