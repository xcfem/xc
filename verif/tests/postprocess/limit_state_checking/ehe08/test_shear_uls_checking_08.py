# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from materials.sections.fiber_section import def_simple_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data
from postprocess import RC_material_distribution
from postprocess import element_section_map
from postprocess.config import default_config
from misc_utils import log_messages as lmsg
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
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= testRCSection.defineMainReinforcement(nominalCover= columnNominalCover, fiStirr= columnStirrupsDiameter, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 7, nRebarsVert= 11)
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
fakeElement.setVerbosityLevel(1) # Restore verbosity level.
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

#Checking shear.
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
limit_state_data.LimitStateData.envConfig= cfg
internalForcesFName= pth+"/../../../aux/internal_forces/intForce_ULS_9565.json"
shutil.copy(internalForcesFName, limit_state_data.shearResistance.getInternalForcesFileName())

# Check limit state.
## Limit state to check.
limitState= limit_state_data.shearResistance
## Build controller.
controller= EHE_limit_state_checking.ShearController(limitStateLabel= limitState.label, solutionProcedureType= CustomNewtonRaphson)
controller.verbose= False # Don't display log messages.
## Perform checking.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y', controller= controller,threeDim= True)

ratio1= abs(meanCFs[0]-0.3834250069696015)/0.3834250069696015
ratio2= abs(meanCFs[1]-0.37791745348202144)/0.37791745348202144

'''
print('meanCFs= ',meanCFs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) and (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
    
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
