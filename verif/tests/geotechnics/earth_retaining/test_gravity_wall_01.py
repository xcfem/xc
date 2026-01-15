# -*- coding: utf-8 -*-
'''
   Gravity Wall test.

Verification test based on the following worked example:

tigalgamuwa.lk/wp-content/uploads/2022/01/30.Gravity-Retaining-Wall-Worked-Examples.pdf
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from geotechnics import frictional_soil as fs
import math
import scipy.integrate
from scipy.constants import g
import geom
import matplotlib
from matplotlib import patches
from matplotlib.collections import PatchCollection
import matplotlib.pyplot as plt
from geotechnics.earth_retaining import gravity_wall

#       ___      _ _    
#      / __| ___(_) |___
#      \__ \/ _ \ | (_-<
#      |___/\___/_|_/__/
# Soils.
## Backfill soil.
gammaMPhi= 1.25 # EC7 table A.2 partial factors for soil parameters.
backfillSoilRho= 18.0e3/g
backfillSoilPhi= 34 # Adjusted to get Ka= 0.31 (see page 2 of the reference document).
backfillSoil= fs.FrictionalSoil(phi= math.radians(backfillSoilPhi), rho= backfillSoilRho, gammaMPhi= gammaMPhi)

backfillSoilDesignPhi= backfillSoil.getDesignPhi()
backfillEarthPressureDelta= backfillSoil.getDesignPhi() # According to paragraph (7) of clause 9.5.1 of EC7.
backfillSoilKa= backfillSoil.Ka_coulomb(a= 0, b= 0, d= backfillEarthPressureDelta, designValue= True)

## Foundation soil.
foundationSoilRho= 20.0e3/g
foundationSoilPhi= 29 # Adjusted to get Ka= 0.37 (see page 2 of the reference document).
foundationSoil= fs.FrictionalSoil(phi= math.radians(foundationSoilPhi), rho= foundationSoilRho, gammaMPhi= gammaMPhi)

foundationSoilDesignPhi= foundationSoil.getDesignPhi()
foundationEarthPressureDelta= foundationSoil.getDesignPhi() # According to paragraph (7) of clause 9.5.1 of EC7.
foundationSoilKa= foundationSoil.Ka_coulomb(a= 0, b= 0, d= foundationEarthPressureDelta, designValue= True)

#        ___              _ _                      _ _ 
#       / __|_ _ __ ___ _(_) |_ _  _  __ __ ____ _| | |
#      | (_ | '_/ _` \ V / |  _| || | \ V  V / _` | | |
#       \___|_| \__,_|\_/|_|\__|\_, |  \_/\_/\__,_|_|_|
#                               |__/                   
# Gravity wall
wallPoints= [geom.Pos2d(0, 0), # 0
             geom.Pos2d(0, -4), # 1
             geom.Pos2d(0, -5), # 2
             geom.Pos2d(-2.6, -5), # 3
             geom.Pos2d(-2.6, -4), # 4
             geom.Pos2d(-2.6, -3), # 5
             geom.Pos2d(-1.8, 0)] # 6

# Contour of the gravity wall.
wallPolygon= geom.Polygon2d(wallPoints)
# 2D polyline representing the contact of the backfill with the wall.
soilWallInterface= [(geom.Segment2d(wallPoints[1], wallPoints[0]), backfillSoil),
                    (geom.Segment2d(wallPoints[2], wallPoints[1]), foundationSoil)]
# 2D segment representing the contact of the wall with the foundation soil.
foundationInterface= (geom.Segment2d(wallPoints[3], wallPoints[2]), foundationSoil)
# 2D segment representing the contact of the passive wedge with the wall.
passivePressureContactSurface= geom.Segment2d(wallPoints[3], wallPoints[4])
# Unit weight of the concrete.
wallUnitWeight= 24e3
gravityWall= gravity_wall.GravityWall(wallContour= wallPolygon, backfillContactSurface= soilWallInterface, passivePressureContactSurface=passivePressureContactSurface, footingContactSurface= foundationInterface, wallUnitWeight= wallUnitWeight)

#         _      _   _             
#        /_\  __| |_(_)___ _ _  ___
#       / _ \/ _|  _| / _ \ ' \(_-<
#      /_/ \_\__|\__|_\___/_||_/__/                                 
# Actions

## Self weight.
permanentLoadSVS= gravityWall.getWallWeightSVS()
wallArea= 1.8*3+0.5*0.8*3+2.6*2
wallWeight= wallArea*24e3
ratio1= (permanentLoadSVS.getResultant().y+wallWeight)/wallWeight
testOK= abs(ratio1)<1e-6

## Earth pressure.
backfillSlope= 0 
k_delta= 1.0 # According to paragraph (7) of clause 9.5.1 of EC7.
earthPressureSVS= gravityWall.getEarthPressureSVS(backfillSlope= backfillSlope, k_delta= k_delta)
sg_backfill_top= 0.0 # vertical stress at the top of the backfill.
sg_backfill_bottom= backfillSoilRho*g*4.0 # vertical stress at the bottom of the backfill.
sg_foundation_soil_top= sg_backfill_bottom
sg_foundation_soil_bottom= sg_backfill_bottom+foundationSoilRho*g*1.0
earthPressureBackfillRef= 0.5*4.0*backfillSoilKa*sg_backfill_bottom
earthPressureFoundationRef= 1.0*foundationSoilKa*(sg_backfill_bottom+sg_foundation_soil_bottom)/2.0
earthPressureRef= earthPressureBackfillRef+earthPressureFoundationRef
ratio2= (earthPressureSVS.getResultant().x+earthPressureRef)/earthPressureRef
testOK= testOK and (abs(ratio2)<1e-6)

## Uniform load on the backfill surface.
pLoad= -20.0e3
uniformLoadSVS= gravityWall.getUniformLoadSVS(uniformLoad= pLoad, backfillSlope= backfillSlope, k_delta= k_delta)
uniformLoadEarthPressureBackfillRef= 4.0*backfillSoilKa*pLoad
uniformLoadEarthPressureFoundationSoilRef= 1.0*foundationSoilKa*pLoad
uniformLoadEarthPressureRef= uniformLoadEarthPressureBackfillRef+uniformLoadEarthPressureFoundationSoilRef
ratio3= (uniformLoadSVS.getResultant().x-uniformLoadEarthPressureRef)/-uniformLoadEarthPressureRef
testOK= testOK and (abs(ratio3)<1e-6)

#        ___              _                 _           
#       / _ \__ _____ _ _| |_ _  _ _ _ _ _ (_)_ _  __ _ 
#      | (_) \ V / -_) '_|  _| || | '_| ' \| | ' \/ _` |
#       \___/ \_/\___|_|  \__|\_,_|_| |_||_|_|_||_\__, |
#                                                 |___/ 
# Overturning safety factor.
overturningComb= [0.9*permanentLoadSVS, 1.1*earthPressureSVS, 1.5*uniformLoadSVS]
O= wallPoints[3]
svsStabilising= 0.9*permanentLoadSVS
Mstab= svsStabilising.getMoment(O)
svsDestabilising= 1.1*earthPressureSVS+1.5*uniformLoadSVS
Mdestab= svsDestabilising.getMoment(O)
overTurningOverdesignFactorRef= -Mstab/Mdestab
overTurningOverdesignFactor= gravityWall.getClassicOverturningSafetyFactorForLoadCombination(pointOfRotation= O, svsList= overturningComb)
ratio4= abs(overTurningOverdesignFactorRef-overTurningOverdesignFactor)/overTurningOverdesignFactorRef
testOK= testOK and (abs(ratio4)<1e-6)
# Fo,R,zml,p= gravityWall.getOverturningSafetyFactor(overturningComb)

#       ___ _ _    _ _           
#      / __| (_)__| (_)_ _  __ _ 
#      \__ \ | / _` | | ' \/ _` |
#      |___/_|_\__,_|_|_||_\__, |
#                          |___/ 
# Sliding safety factor.
## Combination 1
backfillSoil.gammaMPhi= 1.0
foundationSoil.gammaMPhi= 1.0
k_delta= 1.0 # According to paragraph (10) of clause 6.5.3 of EC7.
## Recompute soil thrust with those partial safety factors.
earthPressureSVS= gravityWall.getEarthPressureSVS(backfillSlope= backfillSlope, k_delta= k_delta)
uniformLoadSVS= gravityWall.getUniformLoadSVS(uniformLoad= pLoad, backfillSlope= backfillSlope, k_delta= k_delta)
## Compute over-design factor
slidingComb1= 1.0*permanentLoadSVS+1.35*earthPressureSVS+1.5*uniformLoadSVS
slidingOverDesignFactor1= gravityWall.getSlidingSafetyFactor(svs= slidingComb1, k_delta= 1.0)
## Combination 2
backfillSoil.gammaMPhi= 1.25
foundationSoil.gammaMPhi= 1.25
k_delta= 1.0 # According to paragraph (10) of clause 6.5.3 of EC7.
## Recompute soil thrust with those partial safety factors.
earthPressureSVS= gravityWall.getEarthPressureSVS(backfillSlope= backfillSlope, k_delta= k_delta)
uniformLoadSVS= gravityWall.getUniformLoadSVS(uniformLoad= pLoad, backfillSlope= backfillSlope, k_delta= k_delta)
## Compute over-design factor
slidingComb2= 1.0*permanentLoadSVS+1.0*earthPressureSVS+1.3*uniformLoadSVS
slidingOverDesignFactor2= gravityWall.getSlidingSafetyFactor(svs= slidingComb2, k_delta= 1.0)
slidingOverDesignFactor= min(slidingOverDesignFactor1, slidingOverDesignFactor2)
ratio5= math.sqrt((slidingOverDesignFactor1-1.2600283524258065)**2+(slidingOverDesignFactor2-1.0643126850908375)**2)
testOK= testOK and (abs(ratio5)<1e-6)

#       ___               _              
#      | _ ) ___ __ _ _ _(_)_ _  __ _    
#      | _ \/ -_) _` | '_| | ' \/ _` |   
#      |___/\___\__,_|_| |_|_||_\__, |   
#                             _ |___/    
#       __ __ _ _ __  __ _ __(_) |_ _  _ 
#      / _/ _` | '_ \/ _` / _| |  _| || |
#      \__\__,_| .__/\__,_\__|_|\__|\_, |
#              |_|                  |__/ 
# Bearing capacity.
D= wallPoints[5].y-wallPoints[3].y # Foundation depth.
# Approach 2
backfillSoil.gammaMPhi= 1.0
foundationSoil.gammaMPhi= 1.0
bearingSVS1= 1.0*permanentLoadSVS+1.35*earthPressureSVS+1.35*uniformLoadSVS
CFsg, qEd, qRd= gravityWall.getPressureOnFooting(svs= bearingSVS1, D= D, q= 0.0, NgammaCoef= 1.5)
CFsgRef= 1.3170259848205654
ratio6= abs(CFsg-CFsgRef)/CFsgRef
testOK= testOK and (abs(ratio5)<1e-6)


# print('Soils.')
# print('  Backfill soil.')
# print('    Design value of internal friction of the backfill soil: ', math.degrees(backfillSoilDesignPhi))
# print('    Design wall ground interface parameter of the backfill soil: ', math.degrees(backfillEarthPressureDelta))
# print('    Backfill soil Ka= ', backfillSoilKa)
# print('\n  Foundation soil.')
# print('    Design value of internal friction of the foundation soil: ', math.degrees(foundationSoilDesignPhi))
# print('    Design wall ground interface parameter of the foundation soil: ', math.degrees(foundationEarthPressureDelta))
# print('    Foundation soil Ka= ', foundationSoilKa)
# print('\nActions.')
# print('permanentLoadSVS: ', permanentLoadSVS)
# print('ratio1: ', ratio1)
# print('testOK: ', testOK)
# print('earthPressureRef: ', earthPressureRef)
# print('earthPressureSVS: ', earthPressureSVS)
# print('ratio2: ', ratio2)
# print('testOK: ', testOK)
# print('uniformLoadEarthPressureRef: ', uniformLoadEarthPressureRef)
# print('uniformLoadSVS: ', uniformLoadSVS)
# print('ratio3: ', ratio3)
# print('testOK: ', testOK)
# print('\nLimit state checking.')
# print('Mstab= ', Mstab)
# print('Mdestab= ', Mdestab)
# print('overTurningOverdesignFactorRef= ', overTurningOverdesignFactorRef)
# print('overTurningOverdesignFactor= ', overTurningOverdesignFactor)
# print('ratio4: ', ratio4)
# print('testOK: ', testOK)

# print('slidingOverdesignFactor1= ', slidingOverDesignFactor1)
# print('slidingOverdesignFactor2= ', slidingOverDesignFactor2)
# print('slidingOverdesignFactor= ', slidingOverDesignFactor)
# print('ratio5: ', ratio5)
# print('testOK: ', testOK)

# print('foundation depth: '+str(D)+' m')
# print('CF(bearing)= ', CFsg)
# print('qEd(bearing)= ', qEd/1e3, ' kPa' )
# print('qu(bearing)= ', qRd/1e3, ' kPa' )
# print('testOK: ', testOK)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

