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
from geotechnics import earth_pressure as ep
from actions.earth_pressure import earth_pressure

#                         /
#                       /                   
#                     /  Backfill slope
#      zTopWall --- /
#               |  |
#               |  |
#               |  |
#               |  |
#               |  |  
#               |  |  
#               |  |  
#          +----   ------------+  zTopFooting
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

# Granular fill.
slopeOfBackfillSurface= math.radians(20)
## Design approach 1.
granularFill= ep.RankineSoil(phi= math.radians(32.5), rho= 19e3/g, gammaMPhi= 1.25, beta= slopeOfBackfillSurface) 

# Define wall.
concrete= EC2_materials.C25
steel= EC2_materials.S500B

wall= ng_retaining_wall.RetainingWall(name= 'A.4_worked_example', stemHeight= retainedHeight, stemBottomWidth= stemBottomWidth, stemTopWidth= stemTopWidth, stemBackSlope= 0.0, footingThickness= footingThickness, bToe= bToe, bHeel= bHeel, concrete= concrete, steel= steel)
# Characteristic total self-weight of wall.
wallWeight= concrete.density()*wall.getArea()*g
refWallWeight= 183e3/10*g
ratio1= abs(wallWeight-refWallWeight)/refWallWeight
# Characteristic self-weight of the backfill above the heel.
backfillAboveHeelArea= wall.getBackfillAvobeHeelArea(beta= slopeOfBackfillSurface)
backfillAboveHeelWeight= granularFill.rho*g*backfillAboveHeelArea
refBackfillAboveHeelWeight= 274e3
ratio2= abs(backfillAboveHeelWeight-refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight

# Create wall FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+wall.name, kS= 15e6) # assumed value for subgrade reaction modulus.
preprocessor= wallFEModel.getPreprocessor

#Actions.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','earthPress','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
## Self weight.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
wall.createSelfWeightLoads(rho= concrete.density(),grav= g)
## Earth pressure.
### Backfill soil properties
Ka= (granularFill.Ka(designValue= False), granularFill.Ka(designValue= True))
ratio3= math.sqrt((Ka[0]-0.365)**2+(Ka[1]-0.486)**2)
gSoil= granularFill.rho*g
zBottomSoils=[-1e3]
KSoils= [Ka[1]]
gammaSoils= [gSoil]
zWater= -1e3
gammaWater= 1000*g
vDir= xc.Vector([-math.sin(slopeOfBackfillSurface), -math.cos(slopeOfBackfillSurface)])
### Partial safety factors.
gammaG= 1.35
gammaQ= 1.5
### Set current load case.
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')
### Uniform load on the backfill surface.
unifLoadPressure= earth_pressure.UniformPressureOnBackfill(zGround= 0.0, zBottomSoils= zBottomSoils, KSoils= KSoils, qUnif= 5e3, vDir= vDir)
EaQd= wall.createEarthPressureLoadOnStem(unifLoadPressure)
# print(EaQd)
### Earth pressure on back of wall stem.
backfillPressureModel=  earth_pressure.EarthPressureModel(zGround= 0.0, zBottomSoils= zBottomSoils, KSoils= KSoils, gammaSoils= gammaSoils, zWater= zWater, gammaWater= gammaWater,qUnif=0)
slidingVectorSystem= wall.createBackfillPressures(backfillPressureModel, Delta= 0.0)
### Dead load on the wall heel.
wall.createDeadLoad(heelFillDepth= wall.getBackfillAvobeHeelAvgHeight(beta= granularFill.beta), toeFillDepth= 0.0,rho= granularFill.rho, grav= g)




# Verification of drained strength.

'''
print('bHeel= ',bHeel, 'm')
print('wall weight: ', wallWeight/1e3, 'kN/m')
print('reference wall weight: ', refWallWeight/1e3, 'kN/m')
print(ratio1)
print('weight of the backfill above the wall heel: ', backfillAboveHeelWeight/1e3, 'kN/m')
print(ratio2)
print('Ka= ', Ka)
print(ratio3)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-9 and abs(ratio2)<1e-4 and abs(ratio3)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Graphic output.
# wall.draw()
