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
from materials.sections.fiber_section import def_column_RC_section
from materials import typical_materials
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data
from postprocess import RC_material_distribution
from postprocess import element_section_map
from postprocess import phantom_model
from postprocess.config import default_config
from solution import predefined_solutions
from misc_utils import log_messages as lmsg
import shutil


pierDiameter=1.0
# Equivalent rectangular section
concrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
nomCover= 35e-3 # nominal cover 

# Pier reinforcement.
## Shear reinforcement
shearReinfArea= EHE_materials.Fi12
shearReinfDiam= 12e-3
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.25, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

## Longitudinal reinforcement
rebarDiam= 16e-3
rebarArea= EHE_materials.Fi16
numOfRebars= int(math.ceil(math.pi*(pierDiameter-2*nomCover)/.05))
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(pierDiameter-2*(nomCover+shearReinfDiam)), nominalCover= nomCover)])

# Section geometry
section= def_column_RC_section.RCCircularSection(name='test', sectionDescr= 'circular section', Rext= pierDiameter/2.0, concrType=concrete, reinfSteelType= reinfSteel)
section.mainReinf= mainReinf
section.shReinf= shearReinf


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
fakeElement.tag= 3063
fakeElement.setVerbosityLevel(1) # Restore verbosity level.
xcTotalSet= preprocessor.getSets.getSet('total')
fakeElementSet= xcTotalSet.getElements
# end of the fake stuff.

    
# Store the section in the data structure that the "phantom model" uses
pierRCSects= element_section_map.RCMemberSection('test', [section, section])
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition #sectContainer container
sectContainer.append(pierRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet,setRCSects=pierRCSects)

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
internalForcesFName= pth+"/../../../aux/internal_forces/intForce_ULS_shearResistance.csv"
shutil.copy(internalForcesFName, limit_state_data.shearResistance.getInternalForcesFileName())

# Check limit state.
## Limit state to check.
limitState= EHE_limit_state_checking.shearResistance
## Build controller.
controller= limitState.getController(solutionProcedureType= CustomNewtonRaphson)
controller.verbose= False # Don't display log messages.
## Perform checking.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y', controller= controller,threeDim= True)

# Check results (21/06/2025: update values after fixing error in the EHE08
# shear controller).
ratio1= abs(meanCFs[0]-0.41098330384622167)/0.41098330384622167
ratio2= abs(meanCFs[1]-0.41090892060786455)/0.41090892060786455

'''
print('meanCFs= ',meanCFs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
cfg.cleandirs()  # Clean after yourself.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) and (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
