# -*- coding: utf-8 -*-
''' Check normal stresses of circular section and compare results with those  with those foun in the calculation report of the project:

Proyecto de construcción de plataforma de la línea de alta velocidad de la conexión Madrid – País Vasco – frontera francesa. tramo: Burgos – Vitoria. Subtramo: variante ferroviaria de Burgos - Valle de las Navas. Anejo 11. Apéndice 1. Página 2074.
'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import json
import math
import xc
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from materials import typical_materials
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from postprocess import limit_state_data
from postprocess import RC_material_distribution
from postprocess import element_section_map
from postprocess import phantom_model
from postprocess.config import default_config
from postprocess.control_vars import RCShearControlVars
from postprocess.control_vars import BiaxialBendingControlVars
from postprocess.reports import common_formats as cf
from solution import predefined_solutions
from misc_utils import log_messages as lmsg
import shutil

pileDiameter= 1.5-.05 # EN 1992-1-1 clause 2.3.4.2 (2) 
kf= 1.25 # EN 1992-1-1 clause 2.4.2.5 (2) 
concrete= EC2_materials.EC2Concrete('pileC30', fck= -30e6, gammaC= 1.5*kf)
reinfSteel= EC2_materials.S500B
nomCover= 75e-3 # nominal cover 

# Pile reinforcement.
## Shear reinforcement
shearReinfDiam= 12e-3
shearReinfArea= math.pi*(shearReinfDiam/2.0)**2
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.20, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

## Longitudinal reinforcement
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 28
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(pileDiameter-2*(nomCover+shearReinfDiam)), nominalCover= nomCover)])

# Section geometry
section= def_column_RC_section.RCCircularSection(name='test', sectionDescr= 'circular section', Rext= pileDiameter/2.0, concrType=concrete, reinfSteelType= reinfSteel)
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
fakeElement.tag= 9999
fakeElement.setVerbosityLevel(1) # Restore verbosity level.
xcTotalSet= preprocessor.getSets.getSet('total')
fakeElementSet= xcTotalSet.getElements
# end of the fake stuff.

    
# Store the section in the data structure that the "phantom model" uses
pileRCSects= element_section_map.RCMemberSection('test', [section, section])
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sectContainer= reinfConcreteSectionDistribution.sectionDefinition #sectContainer container
sectContainer.append(pileRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet,setRCSects=pileRCSects)

feProblem.clearAll() #Erase all the fake stuff

#Checking normal stresses.
import os
pth= os.path.dirname(__file__)
#print("pth= ", pth)
if(not pth):
  pth= "."

cfg= default_config.get_temporary_env_config()
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
limit_state_data.LimitStateData.envConfig= cfg
internalForcesFName= pth+"/../../../aux/internal_forces/internal_forces_on_piles_9999.json"
shutil.copy(internalForcesFName, limit_state_data.normalStressesResistance.getInternalForcesFileName())

# Check limit state.
## Limit state to check.
limitState= EC2_limit_state_checking.normalStressesResistance
## Build controller.
controller= limitState.getController()
controller.verbose= False # Don't display log messages.
## Perform checking.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y', controller= controller,threeDim= True)

# Check results.
## The result from the calculation report was CF=0.28 but it was obtained with
## kf= 1.0 and nominal diameter 1.5 m (see EN 1992-1-1 clause 2.3.4.2 (2)
## and clause clause 2.4.2.5 (2). If you change those parametera at the
## begining of this script, you will also obtain that result.
refMeanFC0= 0.3558692507959151
ratio1= abs(meanCFs[0]-refMeanFC0)/refMeanFC0
refMeanFC1= 0.355869250795915
ratio2= abs(meanCFs[1]-refMeanFC1)/refMeanFC1

# print(meanCFs)

cfg.cleandirs()  # Clean after yourself.
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
# # Report results.
# normalStressesCheckResultsData= limitState.getOutputDataBaseDict()
# results= normalStressesCheckResultsData['elementData']['9999']
# for i, key in enumerate(results):
#     sectionResults= eval(results[key])
#     print(str(9999)+', '+str(i)+', CF= '+cf.Factor.format(sectionResults.CF)+' N= '+cf.Force.format(sectionResults.N/1e3)+' kN, My= '+cf.Force.format(sectionResults.My/1e3)+' kN.m, Mz= '+cf.Force.format(sectionResults.Mz/1e3), ' kN.m')
#     # resultDict= sectionResults.getDict())

