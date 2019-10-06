# -*- coding: utf-8 -*-
import math
import csv
import xc_base
import geom
import xc
from materials.sections.fiber_section import defSimpleRCSection as rcs
from materials import typical_materials
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data
from postprocess import RC_material_distribution
from postprocess import element_section_map
from postprocess import phantom_model
from solution import predefined_solutions
from postprocess.config import output_config
from miscUtils import LogMessages as lmsg



diamPilas=1.0
lRectEqPila=round(math.pi**0.5*diamPilas/2.,3)
concrete=EHE_materials.HA30
reinfSteel= EHE_materials.B500S
rnom=35 #recubrimiento nominal 

#   ***Armadura  pilas***
#Armadura longitudinal en cada cara de la pila
lnPil=[16,50]  #armadura longitudinal en cada cara de la pila
# Cercos
nramas_m=3  #nº de ramas en cada dirección
diam_cercos=12
sep_long=250
cercosPil=[diam_cercos,nramas_m,sep_long]
pilasRCSects=rcs.RecordRCSlabBeamSection(name='pilasRCSects',sectionDescr='pilas',concrType=concrete, reinfSteelType=reinfSteel,width=lRectEqPila,depth=lRectEqPila,elemSetName='pilas')

pilasRCSects.dir1PositvRebarRows=[rcs.rebLayer_mm(lnPil[0],lnPil[1],rnom)]
pilasRCSects.dir1NegatvRebarRows=[rcs.rebLayer_mm(lnPil[0],lnPil[1],rnom)]
pilasRCSects.dir2PositvRebarRows=[rcs.rebLayer_mm(lnPil[0],lnPil[1],rnom)]
pilasRCSects.dir2NegatvRebarRows=[rcs.rebLayer_mm(lnPil[0],lnPil[1],rnom)]

pilasRCSects.dir1ShReinfZ=rcs.RecordShearReinforcement(familyName= "sh",nShReinfBranches=cercosPil[1],areaShReinfBranch= math.pi*(cercosPil[0]*1e-3)**2/4.,shReinfSpacing=cercosPil[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
pilasRCSects.dir2ShReinfZ=rcs.RecordShearReinforcement(familyName= "sh",nShReinfBranches=cercosPil[1],areaShReinfBranch= math.pi*(cercosPil[0]*1e-3)**2/4.,shReinfSpacing=cercosPil[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

pilasRCSects.dir1ShReinfY=rcs.RecordShearReinforcement(familyName= "sh",nShReinfBranches=cercosPil[1],areaShReinfBranch= math.pi*(cercosPil[0]*1e-3)**2/4.,shReinfSpacing=cercosPil[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)
pilasRCSects.dir2ShReinfY=rcs.RecordShearReinforcement(familyName= "sh",nShReinfBranches=cercosPil[1],areaShReinfBranch= math.pi*(cercosPil[0]*1e-3)**2/4.,shReinfSpacing=cercosPil[2]*1e-3,angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

# All this is made only to allow the
# creation of the RCMaterialDistribution
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
nodeHandler.dimSpace= 1 # One coordinate for each node.
nodeHandler.numDOFs= 1 # One degree of freedom for each node.
n1= nodeHandler.newNodeXY(1,0)
n2= nodeHandler.newNodeXY(2,0)
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

    
#Store the section in the structure that the "phantom model" uses
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSectionDistribution.sectionDefinition #sections container
pilasRCSects.creaTwoSections()    
sections.append(pilasRCSects)
reinfConcreteSectionDistribution.assign(fakeElementSet,setRCSects=pilasRCSects)

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

limit_state_data.shearResistance.controller= EHE_limit_state_checking.ShearController(limitStateLabel= limit_state_data.shearResistance.label)
limit_state_data.shearResistance.controller.verbose= False # Don't display log messages.
limit_state_data.shearResistance.controller.analysisToPerform=custom_newton_raphson
limit_state_data.LimitStateData.internal_forces_results_directory= pth+'/'
limit_state_data.LimitStateData.check_results_directory= '/tmp/'
limit_state_data.normalStressesResistance.outputDataBaseFileName= 'resVerif'

outCfg=output_config.verifOutVars(listFile='N',calcMeanCF='Y')

(FEcheckedModel,meanFCs)= reinfConcreteSectionDistribution.runChecking(limit_state_data.shearResistance, matDiagType="d",threeDim= True,outputCfg=outCfg)  

ratio1= (meanFCs[0]-0.51922390249173578)/0.51922390249173578
ratio2= (meanFCs[1]-0.51808548496704088)/0.51808548496704088

'''
print('meanFCs= ',meanFCs)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-4) and (ratio2<1e-4):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
