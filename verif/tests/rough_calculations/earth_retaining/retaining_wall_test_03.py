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
ratio0= abs(wallWeight-refWallWeight)/refWallWeight
# Characteristic self-weight of the backfill above the heel.
backfillAboveHeelArea= wall.getBackfillAvobeHeelArea(beta= granularFill.beta)
backfillAboveHeelWeight= granularFill.rho*g*backfillAboveHeelArea
refBackfillAboveHeelWeight= 274e3
ratio1= abs(backfillAboveHeelWeight-refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight

# Create wall FE model.
wallFEModel= wall.createLinearElasticFEModel(prbName= 'Retaining wall '+wall.name, kS= 15e6) # assumed value for subgrade reaction modulus.
preprocessor= wallFEModel.getPreprocessor

#Actions.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','earthPress','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
### Partial safety factors.
gammaG= 1.35
gammaQ= 1.3
## Self weight of the wall.
selfWeight= loadCaseManager.setCurrentLoadCase('selfWeight')
Wselfk= wall.createSelfWeightLoads(rho= concrete.density(),grav= g)
Wselfd= gammaG*Wselfk
### Check characteristic total self-weight of wall:
ratioWselfk= abs(Wselfk.getResultant().y+183e3)/183e3

## Dead load on the heel.
heelFillDepth= wall.getBackfillAvobeHeelAvgHeight(beta= granularFill.beta, zGround= 0.0)
Wfillk= wall.createDeadLoad(heelFillDepth= heelFillDepth, toeFillDepth= 0.0, rho= granularFill.rho, grav= g)
Wfilld= gammaG*Wfillk
### Check characteristic self weight of backfill
ratioWfillk= abs(Wfillk.getResultant().y+refBackfillAboveHeelWeight)/refBackfillAboveHeelWeight

## Earth pressure.
### Backfill soil properties
Ka= (granularFill.Ka(designValue= False), granularFill.Ka(designValue= True))
ratio2= math.sqrt((Ka[0]-0.365)**2+(Ka[1]-0.486)**2)
gSoil= granularFill.rho*g
zBottomSoils=[-1e3]
KSoils= [Ka[1]]
gammaSoils= [gSoil]
zWater= -1e3
gammaWater= 1000*g
vDir= xc.Vector([-math.sin(slopeOfBackfillSurface), -math.cos(slopeOfBackfillSurface)])
### Set current load case.
earthPress= loadCaseManager.setCurrentLoadCase('earthPress')

## Define virtual back.
virtualBack= wall.getVirtualBack(beta= granularFill.beta)
ratio3= abs(virtualBack.getLength()-7.62)/virtualBack.getLength()

### Uniform load on the backfill surface.
qUnif= 5e3
toeEndPos= wall.getToeEndNodePosition()
unifLoadPressure= earth_pressure.UniformPressureOnBackfill(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= KSoils, qUnif= qUnif)
EaQd= gammaQ*wall.createBackfillPressures(pressureModel= unifLoadPressure, Delta= granularFill.beta, beta= granularFill.beta)
virtualBackMidPoint= virtualBack.getMidPoint()
EaQdRef= geom.SlidingVectorsSystem2d(virtualBackMidPoint, -gammaQ*Ka[1]*qUnif*virtualBack.getLength()*geom.Vector2d(math.cos(granularFill.beta), math.sin(granularFill.beta)),0.0)
ratio4a= (EaQd.getResultant()-EaQdRef.getResultant()).getModulus()/EaQdRef.getResultant().getModulus()
MQd= EaQd.getMoment(toeEndPos)
MQdRef= EaQdRef.getMoment(toeEndPos)
ratio4b= abs(MQd-MQdRef)/MQdRef

### Earth pressure on back of wall stem.
backfillPressureModel= earth_pressure.EarthPressureModel(zGround= virtualBack.getFromPoint().y, zBottomSoils= zBottomSoils, KSoils= [Ka[0]], gammaSoils= gammaSoils, zWater= zWater, gammaWater= gammaWater,qUnif=0)
EaGd= gammaG*wall.createBackfillPressures(backfillPressureModel, Delta= granularFill.beta, beta= granularFill.beta)
virtualBackThirdPoint= virtualBack.getToPoint()+virtualBack.getLength()/3.0*geom.Vector2d(0,1)
EaGdRef=  geom.SlidingVectorsSystem2d(virtualBackThirdPoint,-gammaG*Ka[0]*0.5*gSoil*virtualBack.getLength()**2*geom.Vector2d(math.cos(granularFill.beta), math.sin(granularFill.beta)),0.0)
ratio5a= (EaGd.getResultant()-EaGdRef.getResultant()).getModulus()/EaGdRef.getResultant().getModulus()
MGd= EaGd.getMoment(toeEndPos)
MGdRef= EaGdRef.getMoment(toeEndPos)
ratio5b= abs(MGd-MGdRef)/MGdRef
Ead= EaGd+EaQd

# Vertical component of design weight
NEd= Wselfd.getResultant().y+Wfilld.getResultant().y+Ead.getResultant().y

# Compute design thrust.
Ead= EaQd+EaGd
# Compute horizontal component of design thrust.
HEd= Ead.getResultant().x

# Compute vertical component of 


# ### Dead load on the wall heel.
# wall.createDeadLoad(heelFillDepth= wall.getBackfillAvobeHeelAvgHeight(beta= granularFill.beta), toeFillDepth= 0.0,rho= granularFill.rho, grav= g)




# Verification of drained strength.

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
# print('Ka= ', Ka)
# print('ratio2= ', ratio2)

# print('Wselfd= ', Wselfd)
# print('ratioWselfk= ', ratioWselfk)
# print('Wfilld= ', Wfilld)
# print('ratioWfillk= ', ratioWfillk)

# print('EaQd= ', EaQd, 'modulus: ', EaQd.getModulus()/1e3, 'zero moment line: ', EaQd.zeroMomentLine())
# print('EaQdRef= ', EaQdRef, 'modulus: ', EaQdRef.getModulus()/1e3, 'zero moment line: ', EaQdRef.zeroMomentLine())
# print('ratio4a= ', ratio4a)
# print('MQd= ', MQd/1e3)
# print('MQdRef= ', MQdRef/1e3)
# print('ratio4b= ', ratio4b)

# print('EaGd= ', EaGd)
# print('EaGdRef= ', EaGdRef)
# print('ratio5a= ', ratio5a)
# print('MGd= ', MGd/1e3)
# print('MGdRef= ', MGdRef/1e3)
# print('ratio5b= ', ratio5b)
# print('HEd= ', HEd/1e3, 'kN/m')
# print('NEd= ', NEd/1e3, 'kN/m')

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio0)<1e-9 and abs(ratio1)<1e-4 and abs(ratioWselfk)<.05 and abs(ratioWfillk)<.01 and abs(ratio2)<1e-3) and abs(ratio3)<1e-3 and abs(ratio4a)<.01 and abs(ratio4b)<1e-3 and abs(ratio5a)<1e-3 and abs(ratio5b)<.05 :
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
