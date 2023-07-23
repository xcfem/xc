# -*- coding: utf-8 -*-
''' Retaining wall reinforcement checks according to EC2 (home made test).'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import math
from rough_calculations import ng_retaining_wall
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials import typical_materials
from geotechnics import earth_pressure as ep
from geotechnics import frictional_cohesive_soil as fcs
from actions import load_cases
from actions import combinations
from actions.earth_pressure import earth_pressure
from postprocess.config import default_config
from postprocess import limit_state_data as lsd

sectionName= "Test"
title= "Test"

#Geometry
zTopWall=282.69
zTopFooting=280.09

stemBackSlope=0 # H/V
b= 1.0 # length of wall to analyze
stemTopWidth= 0.25
stemHeight=zTopWall-zTopFooting
stemBottomWidth= stemTopWidth+stemBackSlope*stemHeight
bToe= 0.50
bHeel= 2.0-stemBottomWidth
footingThickness= 0.35
cover= 30e-3

#Backfill soil properties
kS= 15e6 #Module de réaction du sol (estimé).
phiS=30#34  #internal frictional angle
rhoS=2000  #density (kg/m3)
backfillDelta=math.radians(18.3)
frontFillDepth= 0.25
zGroundBackfill=0 #Back fill

# Foundation stratified soil properties
hi=[100]
rhoi= [rhoS]
phii= [math.radians(phiS)]
ci=[0e3]


#Loads
gravity=9.81 #Gravity acceleration (m/s2)
HwaterAcc=2.0 # height of water over the top face of the footing

#Materials
concrete= EC2_materials.C30
reinfSteel= EC2_materials.S500B
reinf_types= EC2_limit_state_checking.define_rebar_families(steel= reinfSteel, cover= cover)

wall= ng_retaining_wall.RetainingWall(sectionName,cover,stemBottomWidth,stemTopWidth, stemBackSlope= stemBackSlope, footingThickness= footingThickness, concrete= concrete, steel= reinfSteel,title=title)
wall.stemHeight= stemHeight
wall.bToe= bToe
wall.bHeel= bHeel
wall.concrete= concrete
wall.stemReinforcement.setReinforcement(1, reinf_types['A12_15'].getCopy())  # vert. trasdós (esperas)
wall.stemReinforcement.setReinforcement(2, reinf_types['A12_15'].getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(6, reinf_types['A12_15'].getCopy())  # coronación
wall.stemReinforcement.setReinforcement(11, reinf_types['A12_15'].getCopy()) # horiz. trasdós

wall.stemReinforcement.setReinforcement(4, reinf_types['A12_15'].getCopy()) # vert. intradós (esperas)
wall.stemReinforcement.setReinforcement(5, reinf_types['A12_15'].getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(12, reinf_types['A12_15'].getCopy()) # horiz. intradós

wall.footingReinforcement.setReinforcement(3, reinf_types['A12_15'].getCopy()) # tr. sup. zapata
wall.footingReinforcement.setReinforcement(9, reinf_types['A12_15'].getCopy()) # ln. sup. zapata
wall.footingReinforcement.setReinforcement(7, reinf_types['A12_15'].getCopy()) # tr. inf. zapata
wall.footingReinforcement.setReinforcement(8, reinf_types['A12_15'].getCopy()) # ln. inf. zapata

wall.footingReinforcement.setReinforcement(10, reinf_types['A12_15'].getCopy()) # lateral zapata

# Create wall FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+sectionName, kS= kS)
preprocessor= wallFEModel.getPreprocessor

#Sets.
totalSet= preprocessor.getSets.getSet("total")


#Actions.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','earthPress','earthPressAcc']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Self weight.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
wall.createSelfWeightLoads(rho= concrete.density(),grav= gravity)

## Earth pressure. (drainage ok)
### Backfill soil properties
backfillSoilModel= ep.RankineSoil(phi= math.radians(phiS),rho= rhoS) #Characteristic values.
gSoil= backfillSoilModel.rho*gravity
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')
wall.createDeadLoad(heelFillDepth= wall.stemHeight,toeFillDepth= frontFillDepth,rho= backfillSoilModel.rho, grav= gravity)
Ka= backfillSoilModel.Ka()
backfillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundBackfill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createBackfillPressures(backfillPressureModel, Delta= backfillDelta)
zGroundFrontFill= zGroundBackfill-wall.stemHeight+frontFillDepth #Front fill
frontFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundFrontFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createFrontFillPressures(frontFillPressureModel)

## Accidental: earth pressure failure drainage system.
gSoil= backfillSoilModel.rho*gravity
earthPressAcc= loadCaseManager.setCurrentLoadCase('earthPressAcc')
wall.createDeadLoad(heelFillDepth= wall.stemHeight,toeFillDepth= frontFillDepth,rho= backfillSoilModel.rho, grav= gravity)
Ka= backfillSoilModel.Ka()
backfillPressureModelAcc=  earth_pressure.EarthPressureModel(zGround= zGroundBackfill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater=zGroundBackfill-stemHeight+HwaterAcc, gammaWater= 1000*gravity,qUnif=0)
wall.createBackfillPressures(backfillPressureModelAcc, Delta= backfillDelta)
zGroundFrontFill= zGroundBackfill-wall.stemHeight+frontFillDepth #Front fill
frontFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundFrontFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createFrontFillPressures(frontFillPressureModel)


#Load combinations
combContainer= combinations.CombContainer()

#Quasi-permanent situations.
combContainer.SLS.qp.add('SLS01', '1.0*selfWeight+1.0*earthPress')

#Stability ultimate states. (type 1)
combContainer.ULS.perm.add('ULS01', '0.9*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS02', '1.1*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS03', '0.9*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS04', '1.1*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS05', '0.9*selfWeight+1.00*earthPressAcc')
combContainer.ULS.perm.add('ULS06', '1.1*selfWeight+1.00*earthPressAcc')


#Strength ultimate states. (type 2).
# Earth pressure at rest so 1.35*K0/Ka= 1.35*0.5/0.33= 2.05 -> 2.0
combContainer.ULS.perm.add('ULS07', '1.0*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS08', '1.35*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS09', '1.0*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS10', '1.35*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS11', '1.0*selfWeight+1.00*earthPressAcc')
combContainer.ULS.perm.add('ULS12', '1.35*selfWeight+1.00*earthPressAcc')

#ULS strength analysis.
combContainer.dumpCombinations(preprocessor)
uls_results= wall.performULSAnalysis(['ULS07','ULS08','ULS09','ULS10','ULS11','ULS12'])
wall.setULSInternalForcesEnvelope(uls_results.internalForces)


topFootingDesignBendingMoment= wall.footingReinforcement.wallGeom.internalForcesULS.MdFooting.getAbsMaximum()
topFootingDesignBendingMomentRef= 21.162479010178583e3 # Reference value.
ratio1= abs(topFootingDesignBendingMoment-topFootingDesignBendingMomentRef)/topFootingDesignBendingMomentRef

topFootingDesignShear= wall.footingReinforcement.wallGeom.internalForcesULS.VdFooting.getAbsMaximum()
topFootingDesignShearRef= -9.432470975549364e3
ratio2= abs(topFootingDesignShear-topFootingDesignShearRef)/abs(topFootingDesignShearRef)

# Check RC sections using XC standard routines.

## Compute internal forces.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
lsd.normalStressesResistance.saveAll(combContainer,wall.wallSet) 

## Transfer the reinforcement to the finite element model.
reinfConcreteSectionDistribution= wall.createConcreteSectionDistribution()

## Check normal stresses.
setCalc= wall.wallSet
# variables that control the output of the checking (setCalc,
# appendToResFile .py [defaults to 'N'], listFile .tex [defaults to 'N']
outCfg= lsd.VerifOutVars(setCalc=setCalc,appendToResFile='N',listFile='N',calcMeanCF='N')
limitState= lsd.normalStressesResistance
outCfg.controller= EC2_limit_state_checking.UniaxialBendingNormalStressController(limitState.label)
lsd.normalStressesResistance.check(reinfConcreteSectionDistribution,outCfg, threeDim= False)

## Check results.
wall.modelSpace.readControlVars(inputFileName= cfg.projectDirTree.getVerifNormStrFile())
maxCF= -1e3
for e in wall.wallSet.elements:
    ULS_normalStressesResistanceSect1= e.getProp('ULS_normalStressesResistanceSect1')
    maxCF= max(maxCF,ULS_normalStressesResistanceSect1.CF)
    ULS_normalStressesResistanceSect2= e.getProp('ULS_normalStressesResistanceSect2')
    maxCF= max(maxCF,ULS_normalStressesResistanceSect2.CF)

ratio3= abs(maxCF-0.279256831446576)/0.279256831446576

'''
print(topFootingDesignBendingMoment/1e3)
print(ratio1)
print(topFootingDesignShear/1e3)
print(ratio2)
print(maxCF)
print(ratio3)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-9 and abs(ratio2)<1e-9 and abs(ratio3)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Mathplotlib graphic output.
# wall.draw()

# #########################################################
# # VTK Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(wall.modelSpace)

# # oh.displayFEMesh()
# #Load properties to display:
# argument= 'CF' #Possible arguments: 'CF', 'N', 'Mz'
# oh.displayBeamResult(attributeName=lsd.normalStressesResistance.label, itemToDisp= argument, beamSetDispRes= wall.wallSet, setToDisplay= wall.wallSet)

cfg.cleandirs()  # Clean after yourself.
