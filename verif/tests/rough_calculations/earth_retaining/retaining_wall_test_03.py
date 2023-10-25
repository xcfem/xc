# -*- coding: utf-8 -*-
''' Retaining wall calculation verification test. Inspired on the "A.4 worked example to accompany Chapter 4" of the publication: Eurocode 7: Geotechnical Design Worked examples. Worked examples presented at the Workshop “Eurocode 7: Geotechnical Design” Dublin, 13-14 June, 2013.

https://publications.jrc.ec.europa.eu/repository/handle/JRC85029
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from scipy.constants import g
from materials.ec2 import EC2_materials
from rough_calculations import ng_retaining_wall
from actions import load_cases
from actions import combinations
from geotechnics import earth_pressure as ep
from geotechnics import frictional_cohesive_soil as fcs
from actions.earth_pressure import earth_pressure



#     __      __    _ _                          _            
#     \ \    / /_ _| | |  __ _ ___ ___ _ __  ___| |_ _ _ _  _ 
#      \ \/\/ / _` | | | / _` / -_) _ \ '  \/ -_)  _| '_| || |
#       \_/\_/\__,_|_|_| \__, \___\___/_|_|_\___|\__|_|  \_, |
#                        |___/                           |__/ 
#
#                             /+
#                           /  | V
#                         /    | i
#     Backfill slope -> /      | r            
#                     /        | t
#      zTopWall --- /          | u
#               |  |           | a
#               |  |           | l
#               |  |           |
#               |  |           | b
#               |  |           | a
#               |  |           | c
#               |  |           | k
#          +----   ------------+    <- zTopFooting
#          |Toe          Heel  |
#          +-------------------+
#

# Wall geometry
retainedHeight= 6.0
zTopWall= retainedHeight

## Stem
stemTopWidth= 0.70 # width of the wall at its top.
stemBottomWidth= stemTopWidth # width of the wall at its bottom.

## Foundation.
footingWidth= 3.9 # overall breadth of the base.
footingThickness= 0.8 # thickness of the footing.
bToe= 0.95 # width of the toe.
bHeel= footingWidth-stemBottomWidth-bToe

#  __  __      _           _      _    
# |  \/  |__ _| |_ ___ _ _(_)__ _| |___
# | |\/| / _` |  _/ -_) '_| / _` | (_-<
# |_|  |_\__,_|\__\___|_| |_\__,_|_/__/

# Partial factors (M1)
gammaMPhiM1= 1.0

# Granular fill.
slopeOfBackfillSurface= math.radians(20)
## Design approach 2 (A1+M1+R2).
granularFillM1= ep.RankineSoil(phi= math.radians(32.5), rho= 19e3/g, gammaMPhi= gammaMPhiM1, beta= slopeOfBackfillSurface) 

# Wall materials.
concrete= EC2_materials.C25
steel= EC2_materials.S500B

#  __      __    _ _        _     _        _   
# \ \    / /_ _| | |   ___| |__ (_)___ __| |_ 
# \ \/\/ / _` | | |  / _ \ '_ \| / -_) _|  _|
# \_/\_/\__,_|_|_|  \___/_.__// \___\__|\__|
#                            |__/            

wall= ng_retaining_wall.RetainingWall(name= 'A.4_worked_example', stemHeight= retainedHeight, stemBottomWidth= stemBottomWidth, stemTopWidth= stemTopWidth, stemBackSlope= 0.0, footingThickness= footingThickness, bToe= bToe, bHeel= bHeel, concrete= concrete, steel= steel)

# Characteristic total self-weight of wall.
wallWeight= concrete.density()*wall.getArea()*g
refWallWeight= 183e3/10*g
ratioWallWeight= abs(wallWeight-refWallWeight)/refWallWeight
# Characteristic self-weight of the backfill above the heel.
backfillAboveHeelArea= wall.getBackfillAvobeHeelArea(beta= granularFillM1.beta)
backfillAboveHeelWeight= granularFillM1.rho*g*backfillAboveHeelArea
refBackfillAboveHeelWeight= 274e3
ratioBFWeight= abs(backfillAboveHeelWeight-refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight
wallWeightOK= (abs(ratioWallWeight)<1e-9 and abs(ratioBFWeight)<1e-4)

# Create wall FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+wall.name, kS= 15e6) # assumed value for subgrade reaction modulus.
preprocessor= wallFEModel.getPreprocessor

#    _      _   _             
#   /_\  __| |_(_)___ _ _  ___
#  / _ \/ _|  _| / _ \ ' \(_-<
# /_/ \_\__|\__|_\___/_||_/__/
                             
#Actions.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','earthPress','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

### Partial safety factors on actions (A).
gammaGA1= 1.35 # Set A1.
gammaQA1= 1.5 # Set A1.

#    _      _   _                 ___                                 _     
#   /_\  __| |_(_)___ _ _  ___   | _ \___ _ _ _ __  __ _ _ _  ___ _ _| |_   
#  / _ \/ _|  _| / _ \ ' \(_-<_  |  _/ -_) '_| '  \/ _` | ' \/ -_) ' \  _|_ 
# /_/ \_\__|\__|_\___/_||_/__(_) |_| \___|_| |_|_|_\__,_|_||_\___|_||_\__(_)
                                                                           
## Self weight of the wall.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
Wselfk= wall.createSelfWeightLoads(rho= concrete.density(),grav= g)
### Check characteristic total self-weight of wall:
ratioWselfk= abs(Wselfk.getResultant().y+183e3)/183e3

## Dead load on the heel.
heelFillDepth= wall.getBackfillAvobeHeelAvgHeight(beta= granularFillM1.beta, zGround= 0.0)
Wfillk= wall.createDeadLoad(heelFillDepth= heelFillDepth, toeFillDepth= 0.0, rho= granularFillM1.rho, grav= g)
### Check characteristic self weight of backfill
ratioWfillk= abs(Wfillk.getResultant().y+refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight
weightLoadsOK= (abs(ratioWselfk)<.05 and abs(ratioWfillk)<.01)

## Earth pressure.
### Backfill soil properties
KaM1= granularFillM1.Ka()
ratioKa= abs(KaM1-0.365)/0.365
earthPressureOK= (abs(ratioKa)<1e-2)
gSoil= granularFillM1.rho*g
zBottomSoils=[-1e3]
KSoils= [KaM1]
gammaSoils= [gSoil]
zWater= -1e3
gammaWater= 1000*g
### Set current load case.
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')

## Define virtual back.
virtualBack= wall.getVirtualBack(beta= granularFillM1.beta)
ratioVirtualBack= abs(virtualBack.getLength()-7.62)/virtualBack.getLength()
virtualBackOK= (abs(ratioVirtualBack)<1e-3)

### Earth pressure on back of wall stem.
backfillPressureModel= earth_pressure.EarthPressureModel(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= KSoils, gammaSoils= gammaSoils, zWater= zWater, gammaWater= gammaWater,qUnif=0)
EaGk= wall.createBackfillPressures(backfillPressureModel, Delta= granularFillM1.beta, beta= granularFillM1.beta)
virtualBackThirdPoint= virtualBack.getToPoint()+virtualBack.getLength()/3.0*geom.Vector2d(0,1)
EaGkRef= geom.SlidingVectorsSystem2d(virtualBackThirdPoint,-KaM1*0.5*gSoil*virtualBack.getLength()**2*geom.Vector2d(math.cos(granularFillM1.beta), math.sin(granularFillM1.beta)),0.0)
ratioEaGk= (EaGk.getResultant()-EaGkRef.getResultant()).getModulus()/EaGkRef.getResultant().getModulus()
#### Compare with the result in the book.
EaGkBook= 201e3
ratioEaGkBook= abs(EaGk.getModulus()-EaGkBook)/EaGkBook 
toeEndPos= wall.getToeEndNodePosition()
MGk= EaGk.getMoment(toeEndPos)
MGkRef= EaGkRef.getMoment(toeEndPos)
ratioMGk= abs(MGk-MGkRef)/MGkRef
earthPressureGOK= (abs(ratioEaGk)<1e-3 and abs(ratioEaGkBook)<1e-3 and abs(ratioMGk)<.05)

#    _      _   _                __   __        _      _    _     
#   /_\  __| |_(_)___ _ _  ___   \ \ / /_ _ _ _(_)__ _| |__| |___ 
#  / _ \/ _|  _| / _ \ ' \(_-<_   \ V / _` | '_| / _` | '_ \ / -_)
# /_/ \_\__|\__|_\___/_||_/__(_)   \_/\__,_|_| |_\__,_|_.__/_\___|
                                                                 
### Set current load case.
liveLoad= loadCaseManager.setCurrentLoadCase('liveLoad')
### Uniform load on the backfill surface.
qUnif= 5e3
unifLoadPressure= earth_pressure.UniformPressureOnBackfill(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= KSoils, qUnif= qUnif)
EaQk= wall.createBackfillPressures(pressureModel= unifLoadPressure, Delta= granularFillM1.beta, beta= granularFillM1.beta)
virtualBackMidPoint= virtualBack.getMidPoint()

EaQkRef= geom.SlidingVectorsSystem2d(virtualBackMidPoint, -KaM1*qUnif*virtualBack.getLength()*geom.Vector2d(math.cos(granularFillM1.beta), math.sin(granularFillM1.beta)),0.0)
ratioEaQk= (EaQk.getResultant()-EaQkRef.getResultant()).getModulus()/EaQkRef.getResultant().getModulus()
EaQkBook= 13.9e3
ratioEaQkBook= abs(EaQk.getModulus()-EaQkBook)/EaQkBook 
MQk= EaQk.getMoment(toeEndPos)
MQkRef= EaQkRef.getMoment(toeEndPos)
ratioMQk= abs(MQk-MQkRef)/MQkRef
earthPressureQOK= (abs(ratioEaQk)<1e-3 and abs(ratioEaQkBook)<.005 and abs(ratioMQk)<1e-3)

# ### Uniform load on the heel of the wall
# VQk= wall.createVerticalLoadOnHeel(unifLoadPressure)
# ratioVQk= abs(VQk.getResultant().y+11.3e3)/11.3e3
# MVQk= VQk.getMoment(toeEndPos)
# QQk= qUnif*wall.bHeel
# MVQkRef= QQk*(wall.getFootingWidth()-wall.bHeel/2) # There is an error in the arm lever they use in the book. See page 123.
# ratioMVQk= abs(MVQk+MVQkRef)/MVQkRef
# uniformLoadOnHeelOK= (abs(ratioVQk)<.01 and abs(ratioMVQk)<.1)
uniformLoadOnHeelOK= True # Not considered in the book for approach 2.

# Compute design thrust.
Eak= EaGk+EaQk
EakBook= 214.9e3 # There is an error in the subindices
ratioEakBook= abs(Eak.getResultant().getModulus()-EakBook)/EakBook

# Vertical component of design weight
NEk= Wselfk.getResultant().y+Wfillk.getResultant().y+Eak.getResultant().y
NEkBook= 530.5e3
ratioNEk= abs(NEk+NEkBook)/NEkBook

# Compute horizontal component of design thrust.
HEk= Eak.getResultant().x
HEkBook= 202e3
ratioHEk= abs(HEk+HEkBook)/HEkBook

resultantOK= (ratioEakBook<1e-3) and abs(ratioHEk)<1e-3 and abs(ratioNEk)<.02

# totalLoad= Eak+Wselfk+Wfillk
# MtotalLoad= totalLoad.getMoment(toeEndPos)
# MtotalLoadRef= 332.5e3-1068e3
# print('total load (A1 set): ', totalLoad)
# print('design moment about wall toe (A1 set): ', MtotalLoad/1e3, 'kN.m/m')
# print('reference value of design moment about wall toe (A1 set): ', MtotalLoadRef/1e3, 'kN.m/m')

#   ___           _    _           _   _             
#  / __|___ _ __ | |__(_)_ _  __ _| |_(_)___ _ _  ___
# | (__/ _ \ '  \| '_ \ | ' \/ _` |  _| / _ \ ' \(_-<
# \___\___/_|_|_|_.__/_|_||_\__,_|\__|_\___/_||_/__/
                                                    
combContainer= combinations.CombContainer()

def composeCombinationString(gammaG, gammaQ):
    ''' Compose the combination string corresponding to the arguments.

    :param gammaG: partial factor for permanent loads.
    :param gammaQ: partial factor for variable loads.
    '''
    #loadCaseNames= ['selfWeight','earthPress','liveLoad']
    gammaGStr= str(gammaG)
    gammaQStr= str(gammaQ)
    retval= gammaGStr+'*selfWeight+'+gammaGStr+'*earthPress'
    if(gammaQ!=0.0):
        retval+= '+'+gammaQStr+'*liveLoad'
    return retval

## GEO ultimate states. (type 1)
combContainer.ULS.perm.add('ULS01', composeCombinationString(gammaG= gammaGA1, gammaQ= 0.0))
combContainer.ULS.perm.add('ULS02', composeCombinationString(gammaG= gammaGA1, gammaQ= gammaQA1))
combContainer.dumpCombinations(preprocessor)


#    ___ ___ ___                _  __ _         _   _             
#   / __| __/ _ \  __ _____ _ _(_)/ _(_)__ __ _| |_(_)___ _ _  ___
#  | (_ | _| (_) | \ V / -_) '_| |  _| / _/ _` |  _| / _ \ ' \(_-<
#  \___|___\___/   \_/\___|_| |_|_| |_\__\__,_|\__|_\___/_||_/__/
#                                                                
geoULSCombinations= ['ULS01','ULS02']

# GEO verifications
gammaR2Sliding= 1.1
gammaR2Bearing= 1.4

## Critical state (constant volume) angle of shearing resistance of the soil.
## See clause 6.5.3 (10) of Eurocode 7 part 1. 
phi_cv= math.radians(30)
foundationSoilModel= fcs.FrictionalCohesiveSoil(phi= granularFillM1.phi, c= 0.0, rho= granularFillM1.rho, phi_cv= phi_cv, gammaMPhi= gammaMPhiM1)
## Perform GEO verifications.
sr= wall.performGEOVerifications(geoULSCombinations, foundationSoilModel= foundationSoilModel, toeFillDepth= wall.footingThickness, gammaRSliding= gammaR2Sliding, gammaRBearing= gammaR2Bearing)


## Verification of resistance to sliding
slidingResistanceDegreeOfUtilization= sr.getDegreeOfUtilizationForSliding()
### There is an error in the book. We repeat the operations here:
HEdBook= 274.6e3
NEdBook= 716.9e3
HRdBook= NEdBook*math.tan(math.radians(30))/gammaR2Sliding
slidingResistanceDegreeOfUtilizationBook= HEdBook/HRdBook
### End of book correction.
ratioSDOfU= abs(slidingResistanceDegreeOfUtilization-slidingResistanceDegreeOfUtilizationBook)/slidingResistanceDegreeOfUtilizationBook

## Verification of bearing resistance
bearingResistanceDegreeOfUtilization= sr.getDegreeOfUtilizationForBearingResistance()
bearingResistanceDegreeOfUtilizationRef= 0.8442207731905604
# The value calculated in the book is 99% (and we obtain 84%) the difference
# is mainly due to the fact that the annex D of the EC7 part 1 (informative)
# does not consider the depth factos in the Brinch-Hansen formula.
# bearingResistanceDegreeOfUtilizationBook= 0.99
ratioBRDOfU= abs(bearingResistanceDegreeOfUtilization-bearingResistanceDegreeOfUtilizationRef)/bearingResistanceDegreeOfUtilizationRef

## Verification of resistance to toppling
topplingResistanceDegreeOfUtilization= sr.getDegreeOfUtilizationForOverturning()
topplingResistanceDegreeOfUtilizationBook= 0.32
ratioTDOfU= abs(topplingResistanceDegreeOfUtilization-topplingResistanceDegreeOfUtilizationBook)/topplingResistanceDegreeOfUtilizationBook

geoVerificationsOK= (abs(ratioSDOfU)<.05 and abs(ratioBRDOfU)<1e-3 and abs(ratioTDOfU)<.1)

'''
print('\nCheck computation of virtual back.')
print('virtual back: ', virtualBack)
print('virtual back height: ', virtualBack.getLength())
print('ratioVirtualBack= ', ratioVirtualBack)
print('virtual back OK: ', virtualBackOK)

print('\nCheck computation of the weight of the backfill above the wall heel.')
print('bHeel= ',bHeel, 'm')
print('wall weight: ', wallWeight/1e3, 'kN/m')
print('reference wall weight: ', refWallWeight/1e3, 'kN/m')
print('ratioWallWeight= ',ratioWallWeight)
print('weight of the backfill above the wall heel: ', backfillAboveHeelWeight/1e3, 'kN/m')
print('ratioBFWeight= ',ratioBFWeight)
print('Wall weight OK: ', wallWeightOK)

print('\nCheck earth pressure coefficients.')
print('gSoil= ', gSoil)
print('KaM1= ', KaM1)
print('ratioKa= ', ratioKa)
print('Earte pressure OK: ', earthPressureOK)

print('\nCheck weight loads.')
print('Wselfk= ', Wselfk.getResultant()*1e-3, 'kN/m')
print('ratioWselfk= ', ratioWselfk)
print('Wselfd (A1 set)= ', Wselfk)
print('Wfilld (A1 set)= ', Wfillk)
print('ratioWfillk= ', ratioWfillk)
print('Weight loads OK: ', weightLoadsOK)

print('\nCheck earth pressure due to backfill weight.')
print('EaGk= ', EaGk)
print('EaGkRef= ', EaGkRef)
print('ratioEaGk= ', ratioEaGk)
print('ratioEaGkBook= ', ratioEaGkBook)
print('MGk= ', MGk/1e3)
print('MGkRef= ', MGkRef/1e3)
print('ratioMGk= ', ratioMGk)
print('Earth pressure G OK: ', earthPressureGOK)

print('\nCheck earth pressure due to surface load.')
print('EaQk= ', EaQk, 'modulus: ', EaQk.getModulus()/1e3, 'zero moment line: ', EaQk.zeroMomentLine())
print('EaQkRef= ', EaQkRef, 'modulus: ', EaQkRef.getModulus()/1e3, 'zero moment line: ', EaQkRef.zeroMomentLine())
print('ratioEaQk= ', ratioEaQk)
print('ratioEaQkBook= ', ratioEaQkBook)
print('MQk= ', MQk/1e3)
print('MQkRef= ', MQkRef/1e3)
print('ratioMQk= ', ratioMQk)
print('Earth pressure Q OK: ', earthPressureQOK)

# print('\nCheck uniform load on the heel of the wall.')
# print('VQk= ', VQk)
# print('ratioVQk= ', ratioVQk)
# print('MVQk= ', MVQk/1e3, 'kN.m/m')
# print('MVQkRef=', MVQkRef/1e3, 'kN.m/m')
# print('ratioMVQk= ', ratioMVQk)
# print('Uniform load on heel OK: ', uniformLoadOnHeelOK)

print('\nCheck resultant (A1 set).')
print('Eak= ', Eak)
print('|Eak|= ', Eak.getResultant().getModulus()/1e3,'kN/m')
print('EakBook= ', EakBook)
print('ratioEakBook= ', ratioEakBook)
print('HEk= ', HEk/1e3, 'kN/m')
print('ratioHEk= ', ratioHEk)
print('NEk= ', NEk/1e3, 'kN/m')
print('ratioNEk= ', ratioNEk)
print('Resultant OK: ', resultantOK)

print('\nCheck sliding degree of utilization.')
print('sliding degree of utilization: ', slidingResistanceDegreeOfUtilization)
print('sliding degree of utilization book: ', slidingResistanceDegreeOfUtilizationBook)
print('ratioSDOfU= ', ratioSDOfU)
print('\nCheck bearing degree of utilization.')
print('bearing degree of utilization: ', bearingResistanceDegreeOfUtilization)
print('ratioBRDOfU= ', ratioBRDOfU)
print('\nCheck toppling degree of utilization.')
print('toppling degree of utilization: ', topplingResistanceDegreeOfUtilization)
print('toppling degree of utilization book: ', topplingResistanceDegreeOfUtilizationBook)
print('ratioSDOfU= ', ratioTDOfU)
print('GEO verifications OK: ', geoVerificationsOK)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(wallWeightOK and weightLoadsOK and earthPressureOK and virtualBackOK and earthPressureQOK and uniformLoadOnHeelOK and earthPressureGOK and resultantOK and geoVerificationsOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Graphic output.
# wall.draw()

'''
#########################################################
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(wall.modelSpace)

## Uncomment to display blocks
#oh.displayBlocks()
## Uncomment to display the mesh
#oh.displayFEMesh()

## Uncomment to display the loads
wall.modelSpace.addLoadCaseToDomain('earthPress')
oh.displayLoads()

## Uncomment to display the vertical displacement
#oh.displayDispRot(itemToDisp='uX')
#oh.displayNodeValueDiagram(itemToDisp='uX')

## Uncomment to display the reactions
#oh.displayReactions()

## Uncomment to display the internal force
#oh.displayIntForcDiag('Mz')
'''
