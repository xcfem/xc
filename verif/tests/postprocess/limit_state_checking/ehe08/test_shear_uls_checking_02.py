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
from materials.sections.fiber_section import def_simple_RC_section as rcs
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


piersDiameter=1.0
# Equivalent rectangular section
lRectEqPier=round(math.pi**0.5*piersDiameter/2.,3)
concrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
nomCover=35 # nominal cover 

#   *** Pier reinforcement ***
pierLongReinf=[16,50]  # Longitudinal reinforcement on each side of the pile.
# Stirrups
nlegs_m=3 # number of legs on each direction
shearReinfDiameter=12
shearReinfSpacing=250
pierStirrups= [shearReinfDiameter,nlegs_m,shearReinfSpacing]
piersRCSects= element_section_map.RCSlabBeamSection(name='piersRCSects',sectionDescr='piers',concrType=concrete, reinfSteelType=reinfSteel,width=lRectEqPier,depth=lRectEqPier)

piersRCSects.dir1PositvRebarRows= rcs.LongReinfLayers([rcs.rebLayer_mm(pierLongReinf[0],pierLongReinf[1],nomCover)])
piersRCSects.dir1NegatvRebarRows= rcs.LongReinfLayers([rcs.rebLayer_mm(pierLongReinf[0],pierLongReinf[1],nomCover)])
piersRCSects.dir2PositvRebarRows= rcs.LongReinfLayers([rcs.rebLayer_mm(pierLongReinf[0],pierLongReinf[1],nomCover)])
piersRCSects.dir2NegatvRebarRows= rcs.LongReinfLayers([rcs.rebLayer_mm(pierLongReinf[0],pierLongReinf[1],nomCover)])

# Shear reinforcement.
piersRCSects.dir1ShReinfZ=rcs.ShearReinforcement(familyName= "sh",nShReinfBranches=pierStirrups[1],areaShReinfBranch= math.pi*(pierStirrups[0]*1e-3)**2/4.,shReinfSpacing=pierStirrups[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
piersRCSects.dir2ShReinfZ=rcs.ShearReinforcement(familyName= "sh",nShReinfBranches=pierStirrups[1],areaShReinfBranch= math.pi*(pierStirrups[0]*1e-3)**2/4.,shReinfSpacing=pierStirrups[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

piersRCSects.dir1ShReinfY=rcs.ShearReinforcement(familyName= "sh",nShReinfBranches=pierStirrups[1],areaShReinfBranch= math.pi*(pierStirrups[0]*1e-3)**2/4.,shReinfSpacing=pierStirrups[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
piersRCSects.dir2ShReinfY=rcs.ShearReinforcement(familyName= "sh",nShReinfBranches=pierStirrups[1],areaShReinfBranch= math.pi*(pierStirrups[0]*1e-3)**2/4.,shReinfSpacing=pierStirrups[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

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
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSectionDistribution.sectionDefinition #sections container
sections.append(piersRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet,setRCSects=piersRCSects)

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
meanCFs= limitState.check(setCalc= None, listFile='N',calcMeanCF='Y', controller= controller, threeDim= True, crossSections= reinfConcreteSectionDistribution)

# Check results (21/06/2025: update values after fixing error in the EHE08
# shear controller).
ratio1= abs(meanCFs[0]-0.519223902491736)/0.519223902491736
ratio2= abs(meanCFs[1]-0.42377459585998756)/0.42377459585998756

'''
print('meanCFs= ',meanCFs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

cfg.cleandirs()  # Clean after yourself.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) and (ratio2<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Report sections.
# reinfConcreteSectionDistribution.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
