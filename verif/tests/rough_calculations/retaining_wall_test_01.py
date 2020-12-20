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
from rough_calculations import ng_retaining_wall
import xc_base
import geom
import xc
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials import typical_materials
from geotechnics import earth_pressure as ep
from geotechnics import frictional_cohesional_soil as fcs
from actions import load_cases
from actions import combinations
from actions.earth_pressure import earth_pressure
from postprocess.reports import common_formats as fmt
#from postprocess import output_handler

#Geometry
zTopWall= 9.2 # z coordinate of the wall top.
zTopFooting= 0.0 # z coordinate of the footing upper surface.

stemSlope= 1/10.0 # H/V
b= 1.0   #length of wall to analyze
stemTopWidth= 0.40 # width of the wall at its top.
stemHeight= zTopWall-zTopFooting # height of the stem.
stemBottomWidth= stemTopWidth+stemSlope*stemHeight # width of the wall at its bottom.
bToe= 1.6 # width of the toe.
bHeel= 6.0-stemBottomWidth # width of the heel.
footingThickness= 1.5 # thickness of the footing.
cover= 60e-3 # rebars cover.

#Backfill soil properties
kS= 15e6 # Winkler modulus.
phiS= 30  # internal frictional angle
rhoS= 2000  # density (kg/m3)
backFillDelta= math.radians(18.4)
frontFillDepth= 1.0
zGroundBackFill= -0.2 # back fill

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
pthAux= os.path.dirname(__file__)
if(not pthAux):
  pth= "."
exec(open(pthAux+"../aux/rebar_types.py").read())

sectionName= "mur10m"
wall= ng_retaining_wall.RetainingWall(sectionName,cover,stemBottomWidth,stemTopWidth, stemBackSlope= 1/10.0, footingThickness= footingThickness, concrete= concrete, steel= reinfSteel)
wall.stemHeight= stemHeight
wall.bToe= bToe
wall.bHeel= bHeel
wall.concrete= concrete
#wall.exigeanceFisuration= 'A'
wall.stemReinforcement.setReinforcement(1,A25_10.getCopy('A'))  # vert. trasdós (esperas)
wall.stemReinforcement.setReinforcement(2,A25_10.getCopy('A')) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(11,A16_20.getCopy('A')) #horiz. trasdós

wall.stemReinforcement.setReinforcement(4,A16_20.getCopy('A')) # vert. intradós (esperas)
wall.stemReinforcement.setReinforcement(5,A16_20.getCopy('A')) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(12,A16_20.getCopy('A')) #horiz. intradós

wall.footingReinforcement.setReinforcement(3,A25_10.getCopy('A')) #tr. sup. zapata
wall.footingReinforcement.setReinforcement(9,A16_10.getCopy('A')) # ln. sup. zapata
wall.footingReinforcement.setReinforcement(7,A20_10.getCopy('A')) # tr. inf. zapata
wall.footingReinforcement.setReinforcement(8,A16_20.getCopy('A')) # ln. inf. zapata

wall.stemReinforcement.setReinforcement(6,A12_20.getCopy('A'))  #coronación

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
sr= wall.performStabilityAnalysis(stabilityULSCombinations,foundationSoilModel,sg_adm=0.281e6)

## ULS strength analysis.
uls_results= wall.performULSAnalysis(strengthULSCombinations)
wall.setULSInternalForcesEnvelope(uls_results.internalForces)



err= math.sqrt((sr.Foverturning-5.66890156886)**2+(sr.Fsliding-1.76751713157)**2+(sr.Fbearing-0.520148768619)**2+(sr.FadmPressure-0.906533104128)**2)

'''
print("Overturning: ",sr.Foverturning)
print("Sliding: ",sr.Fsliding)
print("Bearing: ",sr.Fbearing)
print("Allow. press. ratio: ",sr.FadmPressure)
print("error.: ",err)
'''

pth= "/tmp/results/"
if not os.path.exists(pth):
    os.makedirs(pth)
wall.writeResult(pth)
wall.drawSchema(pth)
#notes= ["Overturning: "+fmt.Factor.format(sr.Foverturning), "Sliding: "+fmt.Factor.format(sr.Fsliding), "Bearing: "+fmt.Factor.format(sr.Fbearing), "Allow. press. ratio: "+fmt.Factor.format(sr.FadmPressure)] 
#wall.draw(notes)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-11):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -rf "+pth) # Your garbage you clean it

#########################################################
# Graphic stuff.
#oh= output_handler.OutputHandler(wall.modelSpace)

## Uncomment to display blocks
#oh.displayBlocks()
## Uncomment to display the mesh
#oh.displayFEMesh()

## Uncomment to display the loads
#oh.displayLoads()

## Uncomment to display the vertical displacement
#oh.displayDispRot(itemToDisp='uX')
#oh.displayNodeValueDiagram(itemToDisp='uX')

## Uncomment to display the reactions
#oh.displayReactions()

## Uncomment to display the internal force
#oh.displayIntForcDiag('Mz')

