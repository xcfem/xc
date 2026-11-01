# -*- coding: utf-8 -*-
''' Design of micropiles.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geotechnics import frictional_cohesive_soil as fcs
from geotechnics import terrain_layers as tl
import numpy as np
from scipy.constants import g

# Soils.
unitCSoil= fcs.FrictionalCohesiveSoil(phi= math.radians(27), c= 30e3, rho= 20e3/g, gammaMPhi= 1.5,gammaMc= 1.5)
gravelSoil= fcs.FrictionalCohesiveSoil(phi= math.radians(34), c= 5e3, rho= 20.5e3/g, gammaMPhi= 1.5,gammaMc= 1.5)

#  (-10, 0)                       (+10, 0)
#      +-------+          +---------+
#               \        /
#      (-1.5,-2) +------+ (+1.5,-2)
#          
#          
#            unit C soil
#
#  (-10, -depth)                       (+10, -depth)
#      +----------------------------+
#
#          
#                gravel
#
#

depth= 5.0-1
waterTableDepth= 11.0-1.0

terrain= tl.TerrainStrata()

terrain.appendLayer(soil= unitCSoil, layerSurfacePoints= [geom.Pos3d(-10, 0, 0),geom.Pos3d(-1.5, 0, -2+1e-8), geom.Pos3d(1.5,0,-2+1e-8), geom.Pos3d(10,0,0)])
terrain.appendLayer(soil= gravelSoil, layerSurfacePoints= [geom.Pos3d(-15, 0, -depth+5), geom.Pos3d(-10, 0, -depth), geom.Pos3d(10,0,-depth), geom.Pos3d(15,0,-depth+5)])
terrain.appendWaterTable(waterSurfacePoints= [geom.Pos3d(-15, 0, -waterTableDepth+5), geom.Pos3d(-10, 0, -waterTableDepth), geom.Pos3d(10,0,-waterTableDepth), geom.Pos3d(15,0,-waterTableDepth+5)], waterDensity= 10e3/g)

# Test points
numPoints= 40
depth_i= np.linspace(start= 1, stop= numPoints, num= numPoints, endpoint= True)
testPoints= list()
for depth in depth_i:
    point= geom.Pos3d(0,0,-depth)
    testPoints.append(point)

# Get layers corresponding to each point.
soilLayers= list()
for point in testPoints:
    soilLayers.append(terrain.getLayerAtPoint(point))

# Compute test values.
phiValues= list() # Internal friction angle.
cValues= list() # Cohesion
rhoValues= list() # Density.
k0Values= list() # at-rest earth pressure coefficient
for layer in soilLayers:
    if(layer):
        soil= layer.soil
        phiValues.append(soil.phi)
        cValues.append(soil.c)
        rhoValues.append(soil.rho)
        k0Values.append(soil.K0Jaky())
    else:
        phiValues.append(0.0)
        cValues.append(0.0)
        rhoValues.append(0.0)
        k0Values.append(0.0)
        
verticalStresses= list() # Vertical stress.
for point in testPoints:
    sigma_v= terrain.getWeightVerticalStress(point)
    verticalStresses.append(sigma_v)

horizontalStresses= terrain.getLateralPressuresOnPoints(points= testPoints, unitVectorDirs= None, k= k0Values)

# Skin friction.
pileDiameter= 225e-3
intervalLength= 1.0
pileLateralSurface= math.pi*pileDiameter*intervalLength
Kr= 0.9
skinFrictionValues= list()
for layer, hStress in zip(soilLayers, horizontalStresses):
    skinFriction= 0.0
    if(layer):
        soil= layer.soil
        sg_c= soil.c/soil.gammaMc
        delta= Kr*soil.phi
        sg_p= hStress*math.tan(delta)/soil.gammaMPhi
        skinFriction= sg_c+sg_p
    skinFrictionValues.append(skinFriction)

# Reference values.
phiValuesRef= [0.0, 0.471238898038469, 0.471238898038469, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072, 0.593411945678072]
cValuesRef= [0.0, 30e3, 30e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3, 5e3]
rhoValuesRef= [0.0, 20e3/g, 20e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g, 20.5e3/g]
k0ValuesRef= [0.0, 0.546009500260453, 0.546009500260453, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253, 0.440807096529253]
verticalStressesRef=[0e3, 0e3, 20e3, 40e3, 60.5e3, 81e3, 101.5e3, 122e3, 142.5e3, 163e3, 173.5e3, 184e3, 194.5e3, 205e3, 215.5e3, 226e3, 236.5e3, 247e3, 257.5e3, 268e3, 278.5e3, 289e3, 299.5e3, 310e3, 320.5e3, 331e3, 341.5e3, 352e3, 362.5e3, 373e3, 383.5e3, 394e3, 404.5e3, 415e3, 425.5e3, 436e3, 446.5e3, 457e3, 467.5e3, 478e3]
horizontalStressesRef= [0e3, 0e3, 10.9201900052091e3, 17632.283949331544, 26.6688293400198e3, 35.7053748188695e3, 44.7419202977192e3, 53.7784657765689e3, 62.8150112554186e3, 71.8515567342683e3, 76.4800312478254e3, 81.1085057613826e3, 85.7369802749397e3, 90.3654547884969e3, 94.993929302054e3, 99.6224038156112e3, 104.250878329168e3, 108.879352842726e3, 113.507827356283e3, 118.13630186984e3, 122.764776383397e3, 127.393250896954e3, 132.021725410511e3, 136.650199924068e3, 141.278674437626e3, 145.907148951183e3, 150.53562346474e3, 155.164097978297e3, 159.792572491854e3, 164.421047005411e3, 169.049521518969e3, 173.677996032526e3, 178.306470546083e3, 182.93494505964e3, 187.563419573197e3, 192.191894086754e3, 196.820368600312e3, 201.448843113869e3, 206.077317627426e3, 210.705792140983e3]

skinFrictionValuesRef= [0, 20e3, 23.2871032330342e3, 10285.135772694688, 13.8479344702944e3, 17.4107332026531e3, 20.9735319350118e3, 24.5363306673705e3, 28.0991293997292e3, 31.6619281320879e3, 33.486776263296e3, 35.3116243945041e3, 37.1364725257122e3, 38.9613206569203e3, 40.7861687881285e3, 42.6110169193366e3, 44.4358650505447e3, 46.2607131817528e3, 48.0855613129609e3, 49.910409444169e3, 51.7352575753771e3, 53.5601057065853e3, 55.3849538377934e3, 57.2098019690015e3, 59.0346501002096e3, 60.8594982314177e3, 62.6843463626258e3, 64.5091944938339e3, 66.3340426250421e3, 68.1588907562502e3, 69.9837388874583e3, 71.8085870186664e3, 73.6334351498745e3, 75.4582832810826e3, 77.2831314122907e3, 79.1079795434989e3, 80.932827674707e3, 82.7576758059151e3, 84.5825239371232e3, 86.4073720683313e3]

# Check results.
errPhi= 0.0
for phi, refPhi in zip(phiValues, phiValuesRef):
    errPhi+= (phi-refPhi)**2
errPhi= math.sqrt(errPhi)

errC= 0.0
for c, cRef in zip(cValues, cValuesRef):
    errC+= (c-cRef)**2
errC= math.sqrt(errC)

errRho= 0.0
for rho, refRho in zip(rhoValues, rhoValuesRef):
    errRho+= (rho-refRho)**2
errRho= math.sqrt(errRho)

errK0= 0.0
for k0, refK0 in zip(k0Values, k0ValuesRef):
    errK0+= (k0-refK0)**2
errK0= math.sqrt(errK0)

errVerticalStress= 0.0
for verticalStress, refVerticalStress in zip(verticalStresses, verticalStressesRef):
    err= (verticalStress-refVerticalStress)**2
    errVerticalStress+= err
errVerticalStress= math.sqrt(errVerticalStress)

errHorizontalStress= 0.0
for horizontalStress, refHorizontalStress in zip(horizontalStresses, horizontalStressesRef):
    err= (horizontalStress-refHorizontalStress)**2
    errHorizontalStress+= err
errHorizontalStress= math.sqrt(errHorizontalStress)

errSkinFriction= 0.0
for skinFriction, refSkinFriction in zip(skinFrictionValues, skinFrictionValuesRef):
    err= (skinFriction-refSkinFriction)
    errSkinFriction+= err**2
errSkinFriction= math.sqrt(errSkinFriction)

'''
#print('phiValues= ', phiValues)
#print('phiValuesRef= ', phiValuesRef)
print('errPhi= ', errPhi)
print('errC= ', errC)
print('errRho= ', errRho)
#print('k0Values= ', k0Values)
#print('k0ValuesRef= ', k0ValuesRef)
print('errK0= ', errK0)
#print('verticalStresses= ', verticalStresses)
#print('verticalStressesRef= ', verticalStressesRef)
print('errVerticalStress= ', errVerticalStress)
#print('horizontalStresses= ', horizontalStresses)
#print('horizontalStressesRef= ', horizontalStressesRef)
print('errHorizontalStress= ', errHorizontalStress)
print('skinFrictionValues= ', skinFrictionValues)
print('skinFrictionValuesRef= ', skinFrictionValuesRef)
print('errSkinFriction= ', errSkinFriction)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(errPhi)<1e-8 and abs(errC)<1e-8 and abs(errRho)<1e-8 and abs(errK0)<1e-8 and abs(errVerticalStress)<1e-2 and abs(errHorizontalStress)<1e-3 and abs(errSkinFriction)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
