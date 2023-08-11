# -*- coding: utf-8 -*-
''' Retaining wall calculation verification test (home made).'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import sys
from rough_calculations import ng_retaining_wall
from materials.sia262 import SIA262_materials
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials import typical_materials
from geotechnics import earth_pressure as ep
from geotechnics import frictional_cohesive_soil as fcs
from actions import load_cases
from actions import combinations
from actions.earth_pressure import earth_pressure
#from postprocess.reports import common_formats as fmt

#
#       --  zTopWall
#       | \
#       |  \
#       |  \
#       |   \  stemSlope 
#       |   \  
#       |    \  
#       |    \  
#  +----     ----------+  zTopFooting
#  |Toe          Heel  |
#  +-------------------+
#

# Retaining wall geometry
zTopWall= 9.2 # z coordinate of the wall top.
zTopFooting= 0.0 # z coordinate of the footing upper surface.

## Stem
stemSlope= 1/10.0 # H/V
b= 1.0   #length of wall to analyze
stemTopWidth= 0.40 # width of the wall at its top.
stemHeight= zTopWall-zTopFooting # height of the stem.
stemBottomWidth= stemTopWidth+stemSlope*stemHeight # width of the wall at its bottom.

## Foundation.
bToe= 1.6 # width of the toe.
bHeel= 6.0-stemBottomWidth # width of the heel.
footingThickness= 1.5 # thickness of the footing.
cover= 60e-3 # rebars cover.

# Backfill soil properties
kS= 15e6 # Winkler modulus.
phiS= 30  # internal frictional angle
rhoS= 2000  # density (kg/m3)
backfillDelta= math.radians(18.4)
frontFillDepth= 1.0 # Depth of the soil over the wall toe.
zGroundBackfill= -0.2 # level of the back fill surface with respect to the
                      # top of the wall.

# Foundation stratified soil properties
hi=[1,3,5,8,100]  #cuaternario (QG), QT3L, QT3G,formación  Dueñas
rhoi= [2000,1910,1690,2100,1950]
phii= [math.radians(30),math.radians(25),math.radians(32),math.radians(35),math.radians(24)]
ci=[20e3,2.5e3,5e3,30e3,50e3]

#Loads
gravity=9.81 # Gravity acceleration (m/s2)
HwaterAcc= 1.5

#Materials
concrete= SIA262_materials.c30_37
#concrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S
reinf_types= EHE_limit_state_checking.define_rebar_families(steel= reinfSteel, cover= cover)

sectionName= "mur10m"
wall= ng_retaining_wall.RetainingWall(name= sectionName, concreteCover= cover, stemHeight= stemHeight, stemBottomWidth= stemBottomWidth, stemTopWidth= stemTopWidth, stemBackSlope= stemSlope, footingThickness= footingThickness, bToe= bToe, bHeel= bHeel, concrete= concrete, steel= reinfSteel)

#wall.exigeanceFisuration= 'A'
wall.stemReinforcement.setReinforcement(1, reinf_types['A25_10'].getCopy())  # vert. trasdós (esperas)
wall.stemReinforcement.setReinforcement(2, reinf_types['A25_10'].getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(11, reinf_types['A16_20'].getCopy()) #horiz. trasdós

wall.stemReinforcement.setReinforcement(4, reinf_types['A16_20'].getCopy()) # vert. intradós (esperas)
wall.stemReinforcement.setReinforcement(5, reinf_types['A16_20'].getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(12, reinf_types['A16_20'].getCopy()) #horiz. intradós

wall.footingReinforcement.setReinforcement(3, reinf_types['A25_10'].getCopy()) #tr. sup. zapata
wall.footingReinforcement.setReinforcement(9, reinf_types['A16_10'].getCopy()) # ln. sup. zapata
wall.footingReinforcement.setReinforcement(7, reinf_types['A20_10'].getCopy()) # tr. inf. zapata
wall.footingReinforcement.setReinforcement(8, reinf_types['A16_20'].getCopy()) # ln. inf. zapata

wall.stemReinforcement.setReinforcement(6, reinf_types['A12_20'].getCopy())  #coronación

# Create FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+sectionName, kS= kS)
preprocessor= wallFEModel.getPreprocessor

# Check wireframe stem positions.
stemWFTopPosition= wall.getWFStemTopPosition()
stemWFBottomPosition= wall.getWFStemBottomPosition()
stemWFHeight= wall.getWFStemHeigth()
incXStemPositions= (stemWFBottomPosition.x-stemWFTopPosition.x)
ratioStemPositions= abs(incXStemPositions-stemSlope*stemWFHeight/2.0)

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
## Backfill soil properties
backfillSoilModel= ep.RankineSoil(phi= math.radians(phiS),rho= rhoS) #Characteristic values.
gSoil= backfillSoilModel.rho*gravity
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')
### Dead load on the wall heel.
wall.createDeadLoad(heelFillDepth= wall.stemHeight, toeFillDepth= frontFillDepth,rho= backfillSoilModel.rho, grav= gravity)
Ka= backfillSoilModel.Ka()
### Earth pressure on back of wall stem.
backfillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundBackfill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createBackfillPressures(backfillPressureModel, Delta= backfillDelta)
zGroundFrontFill= zGroundBackfill-wall.stemHeight+frontFillDepth #Front fill
frontFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundFrontFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createFrontFillPressures(frontFillPressureModel)
#Accidental: earth pressure failure drainage system.
gSoil= backfillSoilModel.rho*gravity
earthPressAcc= loadCaseManager.setCurrentLoadCase('earthPressAcc')
wall.createDeadLoad(heelFillDepth= wall.stemHeight,toeFillDepth= frontFillDepth,rho= backfillSoilModel.rho, grav= gravity)
Ka= backfillSoilModel.Ka()
backfillPressureModelAcc=  earth_pressure.EarthPressureModel(zGround= zGroundBackfill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater=zGroundBackfill-stemHeight+HwaterAcc, gammaWater= 1000*gravity,qUnif=0)
wall.createBackfillPressures(backfillPressureModelAcc, Delta= backfillDelta)
zGroundFrontFill= zGroundBackfill-wall.stemHeight+frontFillDepth #Front fill
frontFillPressureModel=  earth_pressure.EarthPressureModel(zGround= zGroundFrontFill, zBottomSoils=[-1e3],KSoils= [Ka], gammaSoils= [gSoil], zWater= -1e3, gammaWater= 1000*gravity,qUnif=0)
wall.createFrontFillPressures(frontFillPressureModel)


#   ___           _    _           _   _             
#  / __|___ _ __ | |__(_)_ _  __ _| |_(_)___ _ _  ___
# | (__/ _ \ '  \| '_ \ | ' \/ _` |  _| / _ \ ' \(_-<
# \___\___/_|_|_|_.__/_|_||_\__,_|\__|_\___/_||_/__/
#
# Load combinations
combContainer= combinations.CombContainer()

## Quasi-permanent situations.
combContainer.SLS.qp.add('SLS01', '1.0*selfWeight+1.0*earthPress')
slsCombinations= ['SLS01']

## Stability ultimate states. (type 1)
combContainer.ULS.perm.add('ULS01', '0.9*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS02', '1.1*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS03', '0.9*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS04', '1.1*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS05', '0.9*selfWeight+1.00*earthPressAcc')
combContainer.ULS.perm.add('ULS06', '1.1*selfWeight+1.00*earthPressAcc')
stabilityULSCombinations= ['ULS01','ULS02','ULS03','ULS04','ULS05','ULS06']

## Strength ultimate states. (type 2).
# Earth pressure at rest so 1.35*K0/Ka= 1.35*0.5/0.33= 2.05 -> 2.0
combContainer.ULS.perm.add('ULS07', '1.0*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS08', '1.35*selfWeight+1.00*earthPress')
combContainer.ULS.perm.add('ULS09', '1.0*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS10', '1.35*selfWeight+1.50*earthPress')
combContainer.ULS.perm.add('ULS11', '1.0*selfWeight+1.00*earthPressAcc')
combContainer.ULS.perm.add('ULS12', '1.35*selfWeight+1.00*earthPressAcc')
strengthULSCombinations= ['ULS07','ULS08','ULS09','ULS10','ULS11','ULS12']

# Limit state checking
## Serviceability analysis.
combContainer.dumpCombinations(preprocessor)
sls_results= wall.performSLSAnalysis(slsCombinations)
wall.setSLSInternalForcesEnvelope(sls_results.internalForces)

## ULS stability analysis.
### Foundation stratified soil properties
stratifiedSoil= fcs.StratifiedSoil(hi,rhoi,phii,ci)
foundationSoilModel= stratifiedSoil.getEquivalentSoil(Beff= 5,gMPhi= 1.2,gMc= 1.5) #Design values.
sr= wall.performStabilityAnalysis(stabilityULSCombinations,foundationSoilModel, toeFillDepth= 1.0, sg_adm=0.281e6, ignoreAdhesion= False, NgammaCoef= 1.5)

## ULS strength analysis.
uls_results= wall.performULSAnalysis(strengthULSCombinations)
wall.setULSInternalForcesEnvelope(uls_results.internalForces)

err= ratioStemPositions**2+math.sqrt((sr.Foverturning-7.126392782772212)**2+(sr.Fsliding-1.5388825671842905)**2+(sr.Fbearing-1.1391242090325084)**2+(sr.FadmPressure-0.9478396196024435)**2)
err= math.sqrt(err)

'''
print('ratioStemPositions= ', ratioStemPositions)
print("Overturning: ",sr.Foverturning)
print("Sliding: ",sr.Fsliding)
print("Bearing: ",sr.Fbearing)
print("Allow. press. ratio: ",sr.FadmPressure)
print("error.: ",err)
'''

pth= "/tmp/tmp_results/"
if not os.path.exists(pth):
    os.makedirs(pth)
wall.writeResult(pth)
wall.drawSchema(pth)
#notes= ["Overturning: "+fmt.Factor.format(sr.Foverturning), "Sliding: "+fmt.Factor.format(sr.Fsliding), "Bearing: "+fmt.Factor.format(sr.Fbearing), "Allow. press. ratio: "+fmt.Factor.format(sr.FadmPressure)] 
#wall.draw(notes)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-9):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.system("rm -rf "+pth) # Your garbage you clean it

# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(wall.modelSpace)

# ## Uncomment to display blocks
# oh.displayBlocks()
# ## Uncomment to display the mesh
# oh.displayFEMesh()

# ## Uncomment to display the loads
# oh.displayLoads()

# ## Uncomment to display the vertical displacement
# oh.displayDispRot(itemToDisp='uX')
# oh.displayNodeValueDiagram(itemToDisp='uX')

# ## Uncomment to display the reactions
# oh.displayReactions()

# ## Uncomment to display the internal force
# oh.displayIntForcDiag('Mz')

