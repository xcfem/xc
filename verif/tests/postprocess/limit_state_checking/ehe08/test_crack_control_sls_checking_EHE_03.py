# -*- coding: utf-8 -*-
''' Test for checking the crack control SLS of a rotated rectangular reinforced concrete column.

    The results seem reasonable but no tests have been found with which to compare them.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking  
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data
from postprocess.config import default_config
from postprocess import RC_material_distribution
from postprocess import element_section_map
from materials import typical_materials
from solution import predefined_solutions
import shutil

# Materials.
structureConcrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S

# Section geometry.
depth= 0.45
width= 1.4
columnNominalCover= 0.04
columnStirrupsDiameter= 12e-3

testRCSection= def_simple_RC_section.RCRectangularSection(name= 'testRCSection', sectionDescr= 'test RC section', concrType= structureConcrete, reinfSteelType= reinfSteel, width= width, depth= depth, swapReinforcementAxes= True)

### Main reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= testRCSection.defineMainReinforcement(nominalCover= columnNominalCover, fiStirr= columnStirrupsDiameter, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
### Shear reinforcement.
testRCSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= columnStirrupsDiameter, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= columnStirrupsDiameter, spacingZ= 0.15)


# All this is made only to allow the
# creation of the RCMaterialDistribution
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
nodeHandler.dimSpace= 1 # One coordinate for each node.
nodeHandler.numDOFs= 1 # One degree of freedom for each node.
n1= nodeHandler.newNodeX(1)
n2= nodeHandler.newNodeX(2)
elementHandler= preprocessor.getElementHandler
elementHandler.dimElem= 1 #Bars defined in a two dimensional space.
fakeMaterial= typical_materials.defElasticMaterial(preprocessor, "fakeMaterial",10)
elementHandler.defaultMaterial= fakeMaterial.name
fakeElement= elementHandler.newElement("ZeroLength",xc.ID([0,0]))
fakeElement.setVerbosityLevel(0) # Avoid warning about set element tag
fakeElement.tag= 9565
fakeElement.setVerbosityLevel(1)
xcTotalSet= preprocessor.getSets.getSet('total')
fakeElementSet= xcTotalSet.getElements
# end of the fake stuff.

    
# Store the section in the data structure that the "phantom model" uses
pierRCSects= element_section_map.RCMemberSection(name='test', templateSections=[testRCSection, testRCSection])
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition #sectContainer container
sectContainer.append(pierRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet, setRCSects=pierRCSects)

feProblem.clearAll() #Erase all the fake stuff

# Checking cracking
class CustomNewtonRaphson(predefined_solutions.PlainNewtonRaphson):
    def __init__(self, prb):
        super(CustomNewtonRaphson, self).__init__(prb,convergenceTestTol= 1e-6)

import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
  pth= "."
cfg= default_config.get_temporary_env_config()
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
# Check limit state.
## Limit state to check.
limitState= limit_state_data.quasiPermanentLoadsCrackControl # Crack control under quasi permanent loads.
limitState.setEnvConfig(cfg= cfg)
internalForcesFName= pth+"/../../../aux/internal_forces/intForce_SLS_9565.json"
shutil.copy(internalForcesFName, limitState.getInternalForcesFileName())

## Build controller.
controller= EHE_limit_state_checking.CrackController(limitState.label, solutionProcedureType= CustomNewtonRaphson, beta= 1.7)
controller.verbose= True #False # Don't display log messages.
## Perform checking.
feProblem.errFileName= "/tmp/erase.err" # Ignore warning messagess about maximum error in computation of the interaction diagram.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y', threeDim= True, controller= controller)
feProblem.errFileName= "cerr" # From now on display errors if any.

ratio1= abs(meanCFs[0]-0.20395294476016704)/0.20395294476016704
ratio2= abs(meanCFs[1]-0.47940824307707813)/0.47940824307707813

# print(meanCFs, ratio1, ratio2)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) & (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# #Load properties to display:
# # Load control variables to display:
# limitState.readControlVars(modelSpace= modelSpace)
# arguments= ['wk', 'CF', 'N', 'My', 'Mz'] # Possible arguments: 'CF','wk', 'N', 'My'
# for arg in arguments:
#     oh.displayBeamResult(attributeName= limitState.label, itemToDisp= arg, setToDisplay= xcTotalSet, beamSetDispRes= xcTotalSet, fileName=None, defFScale=0.0)

# # Matplotlib output
# testRCSection.plot(preprocessor= preprocessor)
    
# # DXF output
# import ezdxf
# doc = ezdxf.new("R2000")
# msp = doc.modelspace()
# testRCSection.writeDXF(modelSpace= msp)
# dxfFileName= fname.removesuffix('.py')
# doc.saveas(dxfFileName+'.dxf')

cfg.cleandirs()  # Clean after yourself.
