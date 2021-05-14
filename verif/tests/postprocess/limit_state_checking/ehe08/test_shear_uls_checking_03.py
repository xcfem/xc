# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import math
import csv
import xc_base
import geom
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
fakeElement.setVerbosityLevel(1)
xcTotalSet= preprocessor.getSets.getSet('total')
fakeElementSet= xcTotalSet.getElements
# end of the fake stuff.

    
# Store the section in the structure that the "phantom model" uses
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
fname= os.path.basename(__file__)

limit_state_data.shearResistance.controller= EHE_limit_state_checking.ShearController(limitStateLabel= limit_state_data.shearResistance.label)
limit_state_data.shearResistance.controller.verbose= False # Don't display log messages.
limit_state_data.shearResistance.controller.solutionProcedureType= CustomNewtonRaphson
cfg=default_config.EnvConfig(language='en', resultsPath= 'tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/',reportPath='./',reportResultsPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'+os.path.splitext(fname)[0]
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
limit_state_data.LimitStateData.envConfig= cfg
shutil.copy(pth+'/intForce_ULS_shearResistance.csv',limit_state_data.shearResistance.getInternalForcesFileName())
#limit_state_data.LimitStateData.internal_forces_results_directory= pth+'/'
#limit_state_data.LimitStateData.check_results_directory= '/tmp/'
#limit_state_data.normalStressesResistance.outputDataBaseFileName= 'resVerif'


outCfg= limit_state_data.VerifOutVars(listFile='N',calcMeanCF='Y')

(FEcheckedModel,meanFCs)= reinfConcreteSectionDistribution.runChecking(limit_state_data.shearResistance, matDiagType="d",threeDim= True,outputCfg=outCfg)  

ratio1= abs(meanFCs[0]-0.80132369462323072)/0.80132369462323072
ratio2= abs(meanFCs[1]-0.80504447847657745)/0.80504447847657745

'''
print('meanFCs= ',meanFCs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
if (ratio1<1e-4) and (ratio2<1e-4):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
