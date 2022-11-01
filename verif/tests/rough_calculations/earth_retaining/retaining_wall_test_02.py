# -*- coding: utf-8 -*-
''' Retaining wall calculation verification test (home made).'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from rough_calculations import ng_retaining_wall
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials import typical_materials
from geotechnics import earth_pressure as ep
from geotechnics import frictional_cohesive_soil as fcs
from actions import load_cases
from actions import combinations
from actions.earth_pressure import earth_pressure

sectionName= "Test"
title= "Test"

#Geometry
zTopWall=282.69
zTopFooting=280.09

stemBackSlope=0 # H/V
b= 1.0   #length of wall to analyze
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
backFillDelta=math.radians(18.3)
frontFillDepth= 0.25
zGroundBackFill=0 #Back fill

# Foundation stratified soil properties (not used)
hi=[1]#,3,5,8,100]  #cuaternario (QG), QT3L, QT3G,formación  Dueñas
rhoi= [rhoS]#,1910,1690,2100,1950]
phii= [math.radians(phiS)]#,math.radians(25),math.radians(32),math.radians(35),math.radians(24)]
ci=[0e3]#,2.5e3,5e3,30e3,50e3]


#Loads
gravity=9.81 #Gravity acceleration (m/s2)
HwaterAcc=2.0 # height of water over the top face of the footing

#Materials
concrete= SIA262_materials.c30_37
#concrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
pthAux= os.path.dirname(__file__)
if(not pthAux):
  pthAux= "."
exec(open(pthAux+"/../../aux/rebar_types.py").read())
#exec(open("./armatures_type.py").read())
wall= ng_retaining_wall.RetainingWall(sectionName,cover,stemBottomWidth,stemTopWidth, stemBackSlope= stemBackSlope, footingThickness= footingThickness, concrete= concrete, steel= reinfSteel,title=title)
wall.stemHeight= stemHeight
wall.bToe= bToe
wall.bHeel= bHeel
wall.concrete= concrete
wall.stemReinforcement.setReinforcement(1,A12_15.getCopy())  # vert. trasdós (esperas)
wall.stemReinforcement.setReinforcement(2,A12_15.getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(6,A12_15.getCopy())  #coronación
wall.stemReinforcement.setReinforcement(11,A12_15.getCopy()) #horiz. trasdós

wall.stemReinforcement.setReinforcement(4,A12_15.getCopy()) # vert. intradós (esperas)
wall.stemReinforcement.setReinforcement(5,A12_15.getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(12,A12_15.getCopy()) #horiz. intradós

wall.footingReinforcement.setReinforcement(3,A12_15.getCopy()) #tr. sup. zapata
wall.footingReinforcement.setReinforcement(9,A12_15.getCopy()) # ln. sup. zapata
wall.footingReinforcement.setReinforcement(7,A12_15.getCopy()) # tr. inf. zapata
wall.footingReinforcement.setReinforcement(8,A12_15.getCopy()) # ln. inf. zapata

wall.footingReinforcement.setReinforcement(10,A12_15.getCopy()) # lateral zapata

wallFEModel= wall.createFEProblem('Retaining wall '+sectionName)
preprocessor= wallFEModel.getPreprocessor
nodes= preprocessor.getNodeHandler

#Soils
kX= typical_materials.defElasticMaterial(preprocessor, "kX",kS/10.0)
kY= typical_materials.defElasticMaterial(preprocessor, "kY",kS)
#kY= typical_materials.defElastNoTensMaterial(preprocessor, "kY",kS)
#Backfill soil properties
backFillSoilModel= ep.RankineSoil(phi= math.radians(phiS),rho= rhoS) #Characteristic values.
#Foundation stratified soil properties
stratifiedSoil= fcs.StratifiedSoil(hi,rhoi,phii,ci)

foundationSoilModel= stratifiedSoil.getEquivalentSoil(Beff= 5,gMPhi= 1.2,gMc= 1.5) #Design values.

#Mesh.
wall.genMesh(nodes,[kX,kY])

#Sets.
totalSet= preprocessor.getSets.getSet("total")


#Actions.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','earthPress','earthPressAcc']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

#Self weight.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
wall.createSelfWeightLoads(rho= concrete.density(),grav= gravity)

# Earth pressure. (drainage ok)
gSoil= backFillSoilModel.rho*gravity
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')
wall.createDeadLoad(heelFillDepth= wall.stemHeight,toeFillDepth= frontFillDepth,rho= backFillSoilModel.rho, grav= gravity)
Ka= backFillSoilModel.Ka()
backFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundBackFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createBackFillPressures(backFillPressureModel, Delta= backFillDelta)
zGroundFrontFill= zGroundBackFill-wall.stemHeight+frontFillDepth #Front fill
frontFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundFrontFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createFrontFillPressures(frontFillPressureModel)

#Accidental: earth pressure failure drainage system.
gSoil= backFillSoilModel.rho*gravity
earthPressAcc= loadCaseManager.setCurrentLoadCase('earthPressAcc')
wall.createDeadLoad(heelFillDepth= wall.stemHeight,toeFillDepth= frontFillDepth,rho= backFillSoilModel.rho, grav= gravity)
Ka= backFillSoilModel.Ka()
backFillPressureModelAcc=  earth_pressure.EarthPressureModel(zGround= zGroundBackFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater=zGroundBackFill-stemHeight+HwaterAcc, gammaWater= 1000*gravity,qUnif=0)
wall.createBackFillPressures(backFillPressureModelAcc, Delta= backFillDelta)
zGroundFrontFill= zGroundBackFill-wall.stemHeight+frontFillDepth #Front fill
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

'''
print(topFootingDesignBendingMoment/1e3)
print(ratio1)
print(topFootingDesignShear/1e3)
print(ratio2)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-9 and abs(ratio2)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
