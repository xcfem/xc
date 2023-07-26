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

# Partial factors (M)
gammaMPhiM1= 1.0
gammaMPhiM2= 1.25

# Granular fill.
slopeOfBackfillSurface= math.radians(20)
## Design approach 1.
granularFillM2= ep.RankineSoil(phi= math.radians(32.5), rho= 19e3/g, gammaMPhi= gammaMPhiM2, beta= slopeOfBackfillSurface) 

# Define wall.
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
ratio0= abs(wallWeight-refWallWeight)/refWallWeight
# Characteristic self-weight of the backfill above the heel.
backfillAboveHeelArea= wall.getBackfillAvobeHeelArea(beta= granularFillM2.beta)
backfillAboveHeelWeight= granularFillM2.rho*g*backfillAboveHeelArea
refBackfillAboveHeelWeight= 274e3
ratio1= abs(backfillAboveHeelWeight-refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight

# Create wall FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+wall.name, kS= 15e6) # assumed value for subgrade reaction modulus.
preprocessor= wallFEModel.getPreprocessor

toeEndPos= wall.getToeEndNodePosition()


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
gammaGA2= 1.0 # Set A2.
gammaQA1= 1.5 # Set A1.
gammaQA2= 1.3 # Set A2.

#    _      _   _                 ___                                 _     
#   /_\  __| |_(_)___ _ _  ___   | _ \___ _ _ _ __  __ _ _ _  ___ _ _| |_   
#  / _ \/ _|  _| / _ \ ' \(_-<_  |  _/ -_) '_| '  \/ _` | ' \/ -_) ' \  _|_ 
# /_/ \_\__|\__|_\___/_||_/__(_) |_| \___|_| |_|_|_\__,_|_||_\___|_||_\__(_)
                                                                           
## Self weight of the wall.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
Wselfk= wall.createSelfWeightLoads(rho= concrete.density(),grav= g)
WselfdA1= gammaGA1*Wselfk
### Check characteristic total self-weight of wall:
ratioWselfk= abs(Wselfk.getResultant().y+183e3)/183e3

## Dead load on the heel.
heelFillDepth= wall.getBackfillAvobeHeelAvgHeight(beta= granularFillM2.beta, zGround= 0.0)
Wfillk= wall.createDeadLoad(heelFillDepth= heelFillDepth, toeFillDepth= 0.0, rho= granularFillM2.rho, grav= g)
WfilldA1= gammaGA1*Wfillk
### Check characteristic self weight of backfill
ratioWfillk= abs(Wfillk.getResultant().y+refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight

## Earth pressure.
### Backfill soil properties
(KaM1, KaM2)= (granularFillM2.Ka(designValue= False), granularFillM2.Ka(designValue= True))
ratio2= math.sqrt((KaM1-0.365)**2+(KaM2-0.486)**2)
gSoil= granularFillM2.rho*g
zBottomSoils=[-1e3]
KSoils= [KaM1]
gammaSoils= [gSoil]
zWater= -1e3
gammaWater= 1000*g
vDir= xc.Vector([-math.sin(slopeOfBackfillSurface), -math.cos(slopeOfBackfillSurface)])
### Set current load case.
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')

## Define virtual back.
virtualBack= wall.getVirtualBack(beta= granularFillM2.beta)
ratio3= abs(virtualBack.getLength()-7.62)/virtualBack.getLength()

### Earth pressure on back of wall stem.
backfillPressureModel= earth_pressure.EarthPressureModel(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= KSoils, gammaSoils= gammaSoils, zWater= zWater, gammaWater= gammaWater,qUnif=0)
EaGk= wall.createBackfillPressures(backfillPressureModel, Delta= granularFillM2.beta, beta= granularFillM2.beta)
EaGdA1= gammaGA1*EaGk
virtualBackThirdPoint= virtualBack.getToPoint()+virtualBack.getLength()/3.0*geom.Vector2d(0,1)
EaGkRef= geom.SlidingVectorsSystem2d(virtualBackThirdPoint,-KaM1*0.5*gSoil*virtualBack.getLength()**2*geom.Vector2d(math.cos(granularFillM2.beta), math.sin(granularFillM2.beta)),0.0)
EaGdA1Ref= gammaGA1*EaGkRef
ratioEaGdA1= (EaGdA1.getResultant()-EaGdA1Ref.getResultant()).getModulus()/EaGdA1Ref.getResultant().getModulus()
MGdA1= EaGdA1.getMoment(toeEndPos)
MGdA1Ref= EaGdA1Ref.getMoment(toeEndPos)
ratioMGdA1= abs(MGdA1-MGdA1Ref)/MGdA1Ref

#    _      _   _                __   __        _      _    _     
#   /_\  __| |_(_)___ _ _  ___   \ \ / /_ _ _ _(_)__ _| |__| |___ 
#  / _ \/ _|  _| / _ \ ' \(_-<_   \ V / _` | '_| / _` | '_ \ / -_)
# /_/ \_\__|\__|_\___/_||_/__(_)   \_/\__,_|_| |_\__,_|_.__/_\___|
                                                                 
### Set current load case.
earthPress= loadCaseManager.setCurrentLoadCase('liveLoad')
### Uniform load on the backfill surface.
qUnif= 5e3
unifLoadPressure= earth_pressure.UniformPressureOnBackfill(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= KSoils, qUnif= qUnif)
EaQk= wall.createBackfillPressures(pressureModel= unifLoadPressure, Delta= granularFillM2.beta, beta= granularFillM2.beta)
EaQdA1= gammaQA1*EaQk
virtualBackMidPoint= virtualBack.getMidPoint()

EaQkRef= geom.SlidingVectorsSystem2d(virtualBackMidPoint, -KaM1*qUnif*virtualBack.getLength()*geom.Vector2d(math.cos(granularFillM2.beta), math.sin(granularFillM2.beta)),0.0)
EaQdA1Ref= gammaQA1*EaQkRef
ratioEaQdA1= (EaQdA1.getResultant()-EaQdA1Ref.getResultant()).getModulus()/EaQdA1Ref.getResultant().getModulus()
MQdA1= EaQdA1.getMoment(toeEndPos)
MQdA1Ref= EaQdA1Ref.getMoment(toeEndPos)
ratioMQdA1= abs(MQdA1-MQdA1Ref)/MQdA1Ref

Ead= EaGdA1+EaQdA1

# Vertical component of design weight
NEd= WselfdA1.getResultant().y+WfilldA1.getResultant().y+Ead.getResultant().y
ratioNEd= abs(NEd+716.9e3)/716.9e3

# Compute design thrust.
Ead= EaQdA1+EaGdA1
# Compute horizontal component of design thrust.
HEd= Ead.getResultant().x
ratioHEd= abs(HEd+274.6e3)/274.6e3



#   ___           _    _           _   _             
#  / __|___ _ __ | |__(_)_ _  __ _| |_(_)___ _ _  ___
# | (__/ _ \ '  \| '_ \ | ' \/ _` |  _| / _ \ ' \(_-<
# \___\___/_|_|_|_.__/_|_||_\__,_|\__|_\___/_||_/__/
                                                    
combContainer= combinations.CombContainer()

def composeCombnationString(gammaG, gammaQ):
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
combContainer.ULS.perm.add('ULS01', composeCombnationString(gammaG= gammaGA1, gammaQ= 0.0))
combContainer.ULS.perm.add('ULS02', composeCombnationString(gammaG= gammaGA1, gammaQ= gammaQA1))
combContainer.dumpCombinations(preprocessor)

geoULSCombinations= ['ULS01','ULS02']
foundationSoilModel= fcs.FrictionalCohesiveSoil(phi= granularFillM2.phi, c= 0.0, rho= granularFillM2.rho, gammaMPhi= gammaMPhiM1) 
sr= wall.performStabilityAnalysis(geoULSCombinations, foundationSoilModel,sg_adm= None)

slidingDegreeOfUtilization= sr.getDegreeOfUtilizationForSliding()
#print(slidingDegreeOfUtilization)




# print('virtual back: ', virtualBack)
# print('virtual back height: ', virtualBack.getLength())
# print('ratio3= ', ratio3)
# '''
# print('bHeel= ',bHeel, 'm')
# print('wall weight: ', wallWeight/1e3, 'kN/m')
# print('reference wall weight: ', refWallWeight/1e3, 'kN/m')
# print('ratio0= ',ratio0)
# print('weight of the backfill above the wall heel: ', backfillAboveHeelWeight/1e3, 'kN/m')
# print('ratio1= ',ratio1)
# '''
# print(Wselfk.getResultant()*1e-3, ratioWselfk)
# print('gSoil= ', gSoil)
# print('KaM1= ', KaM1)
# print('KaM2= ', KaM2)
# print('ratio2= ', ratio2)

# print('Wselfd (A1 set)= ', WselfdA1)
# print('ratioWselfk= ', ratioWselfk)
# print('Wfilld (A1 set)= ', WfilldA1)
# print('ratioWfillk= ', ratioWfillk)

# print('EaQdA1= ', EaQdA1, 'modulus: ', EaQdA1.getModulus()/1e3, 'zero moment line: ', EaQdA1.zeroMomentLine())
# print('EaQdA1Ref= ', EaQdA1Ref, 'modulus: ', EaQdA1Ref.getModulus()/1e3, 'zero moment line: ', EaQdA1Ref.zeroMomentLine())
# print('ratioEaQdA1= ', ratioEaQdA1)
# print('MQd (A1 set)= ', MQdA1/1e3)
# print('MQdRef (A1 set)= ', MQdA1Ref/1e3)
# print('ratioMQd (A1 set)= ', ratioMQdA1)

# print('EaGd (A1 set)= ', EaGdA1)
# print('EaGdRef (A1 set)= ', EaGdA1Ref)
# print('ratioEaGd (A1 set)= ', ratioEaGdA1)
# print('MGd (A1 set)= ', MGdA1/1e3)
# print('MGdRef (A1 set)= ', MGdA1Ref/1e3)
# print('ratioMGd (A1 set)= ', ratioMGdA1)
# print('HEd= ', HEd/1e3, 'kN/m')
# print('ratioHEd= ', ratioHEd)
# print('NEd= ', NEd/1e3, 'kN/m')
# print('ratioNEd= ', ratioNEd)


from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio0)<1e-9 and abs(ratio1)<1e-4 and abs(ratioWselfk)<.05 and abs(ratioWfillk)<.01 and abs(ratio2)<1e-3) and abs(ratio3)<1e-3 and abs(ratioEaQdA1)<.01 and abs(ratioMQdA1)<1e-3 and abs(ratioEaGdA1)<1e-3 and abs(ratioMGdA1)<.05 and abs(ratioHEd)<.05 and abs(ratioNEd)<.02:
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
