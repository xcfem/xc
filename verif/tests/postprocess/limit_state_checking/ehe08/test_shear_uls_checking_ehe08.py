# -*- coding: utf-8 -*-
import math
import csv
import xc_base
import geom
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
concrete=EHE_materials.HA30
reinfSteel= EHE_materials.B500S
nomCover=35 # nominal cover 

#   *** Pier reinforcement ***
pierLongReinf=[16,50]  # Longitudinal reinforcement on each side of the pile.
# Stirrups
nlegs_m=3 # number of legs on each direction
diam_cercos=12
sep_long=250
pierStirrups=[diam_cercos,nlegs_m,sep_long]
piersRCSects=rcs.RCSlabBeamSection(name='piersRCSects',sectionDescr='piers',concrType=concrete, reinfSteelType=reinfSteel,width=lRectEqPier,depth=lRectEqPier,elemSetName='piers')

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
elementHandler.defaultMaterial= "fakeMaterial"
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
piersRCSects.creaTwoSections()    
sections.append(piersRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet,setRCSects=piersRCSects)

feProblem.clearAll() #Erase all the fake stuff

#Checking shear.
def custom_newton_raphson(prb):
    solution=  predefined_solutions.SolutionProcedure()
    solution.convergenceTestTol= 1e-6
    return solution.simpleNewtonRaphson(prb)

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
fname= os.path.basename(__file__)

limit_state_data.shearResistance.controller= EHE_limit_state_checking.ShearController(limitStateLabel= limit_state_data.shearResistance.label)
limit_state_data.shearResistance.controller.verbose= False # Don't display log messages.
limit_state_data.shearResistance.controller.analysisToPerform=custom_newton_raphson
cfg=default_config.EnvConfig(language='en',intForcPath= 'results/internalForces/',verifPath= 'results/verifications/',annexPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'+os.path.splitext(fname)[0]
cfg.projectDirTree.createTree() # To allow copying existing internal force data into.
limit_state_data.LimitStateData.envConfig= cfg
shutil.copy(pth+'/intForce_ULS_shearResistance.csv',limit_state_data.shearResistance.getInternalForcesFileName())
#limit_state_data.LimitStateData.internal_forces_results_directory= pth+'/'
#limit_state_data.LimitStateData.check_results_directory= '/tmp/'
#limit_state_data.normalStressesResistance.outputDataBaseFileName= 'resVerif'

outCfg= limit_state_data.VerifOutVars(listFile='N',calcMeanCF='Y')

(FEcheckedModel,meanFCs)= reinfConcreteSectionDistribution.runChecking(limit_state_data.shearResistance, matDiagType="d",threeDim= True,outputCfg=outCfg)  

ratio1= (meanFCs[0]-0.51922390249173578)/0.51922390249173578
ratio2= (meanFCs[1]-0.51808548496704088)/0.51808548496704088

'''
print('meanFCs= ',meanFCs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
if (ratio1<1e-4) and (ratio2<1e-4):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
