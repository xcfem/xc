# -*- coding: utf-8 -*-
''' Test class to store the values of the geotechnical parameters along the 
    shafts of a set of piles.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import os
import math
from geotechnics.foundations import soil_sampled_data

                
pgpSet= soil_sampled_data.SoilSampledDataSet()

# Read the soil data.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
accelFilePath= pth+'/../../../aux/soil_data/pile_geotechnical_parameters.json'
pgpSet.readFromFile(accelFilePath)

pileCode= 'P-11'
depths= [2.0*i for i in range(1, 38)]
depths.insert(8, 17.0)
p11SoilData= pgpSet.getSoilSampledData(pileCode)

# Check cohesion values.
c_ref_i= [20e3, 20e3, 20e3, 20e3, 20e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3, 35e3]
err_c= 0.0
for depth, c_ref in zip(depths, c_ref_i):
    c= p11SoilData.getCohesionAtDepth(depth)
    err_c+= (c-c_ref)**2
err_c= math.sqrt(err_c)
testOK= (err_c<1e-4)

# Check internal friction angle values.
phi_ref_i= 5*[17.0]+33*[24.0]
err_phi= 0.0 
for depth, phi_ref in zip(depths, phi_ref_i):
    phi= p11SoilData.getInternalFrictionAngleAtDepth(depth)
    err_phi+= (phi-math.radians(phi_ref))**2
err_phi= math.sqrt(err_phi)
testOK&= (err_phi<1e-4)

# Check skin friction resistance.
## Characteristic values.
rsk_ref_i= [281.7e3, 600.4e3, 956e3, 1348.7e3, 1778.4e3, 2467.8e3, 3202.4e3, 3982.2e3, 4377.7e3, 4784.6e3, 5654.7e3, 6569.9e3, 7530.4e3, 8536e3, 9586.8e3, 10682.7e3, 11813.7e3, 12944.7e3, 14075.7e3, 15206.6e3, 16337.6e3, 17468.6e3, 18599.6e3, 19730.5e3, 20861.5e3, 21992.5e3, 23123.4e3, 24254.4e3, 25385.4e3, 26516.4e3, 27647.3e3, 28778.3e3, 29909.3e3, 31040.3e3, 32171.2e3, 33302.2e3, 34433.2e3, 35564.2e3]

err_rsk= 0.0 # Root Mean Squared Relative Error
rsk_accum= 0.0
for depth, rsk_ref in zip(depths, rsk_ref_i):
    rsk_accum= p11SoilData.getAccumulatedPileSkinFrictionBearingCapacityAtDepth(depth, characteristic= True)
    err= (rsk_accum-rsk_ref)/rsk_ref
    err_rsk+= err**2
err_rsk= math.sqrt(err_rsk)/len(rsk_ref_i)
testOK&= (err_rsk<1e-4)

## Design values.
rsd_ref_i= [182.9e3, 389.9e3, 620.8e3, 875.8e3, 1154.8e3, 1602.5e3, 2079.5e3, 2585.8e3, 2842.7e3, 3106.9e3, 3671.9e3, 4266.2e3, 4889.9e3, 5542.9e3, 6225.2e3, 6936.8e3, 7671.2e3, 8405.6e3, 9140e3, 9874.4e3, 10608.8e3, 11343.2e3, 12077.6e3, 12812e3, 13546.4e3, 14280.8e3, 15015.2e3, 15749.6e3, 16484e3, 17218.4e3, 17952.8e3, 18687.2e3, 19421.6e3, 20156e3, 20890.4e3, 21624.8e3, 22359.2e3, 23093.6e3]

err_rsd= 0.0 # Root Mean Squared Relative Error
rsd_accum= 0.0
for depth, rsd_ref in zip(depths, rsd_ref_i):
    rsd_accum= p11SoilData.getAccumulatedPileSkinFrictionBearingCapacityAtDepth(depth, characteristic= False)
    err= (rsd_accum-rsd_ref)/rsd_ref
    err_rsd+= err**2
err_rsd= math.sqrt(err_rsd)/len(rsd_ref_i)
testOK&= (err_rsd<1e-4)

# Check end bearing capacity.
## Characteristic values.
rbk_ref_i= [872.8e3, 1117.6e3, 1362.4e3, 1607.2e3, 1852e3, 4677e3, 5169.7e3, 5662.3e3, 5908.7e3, 6155e3, 6647.7e3, 7140.3e3, 7633e3, 8125.7e3, 8618.3e3, 9111e3, 9603.6e3, 10096.3e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3, 10589e3]

err_rbk= 0.0 # Root Mean Squared Relative Error
rbk_accum= 0.0
for depth, rbk_ref in zip(depths, rbk_ref_i):
    rbk= p11SoilData.getPileEndBearingCapacityAtDepth(depth, characteristic= True)
    err= (rbk-rbk_ref)/rbk_ref
    err_rbk+= err**2
err_rbk= math.sqrt(err_rbk)/len(rbk_ref_i)
testOK&= (err_rbk<1e-4)

## Design values.
rbd_ref_i= [461.8e3, 591.3e3, 720.8e3, 850.4e3, 979.9e3, 2474.6e3, 2735.3e3, 2995.9e3, 3126.3e3, 3256.6e3, 3517.3e3, 3778e3, 4038.6e3, 4299.3e3, 4560e3, 4820.6e3, 5081.3e3, 5342e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3, 5602.6e3]

err_rbd= 0.0 # Root Mean Squared Relative Error
rbd_accum= 0.0
for depth, rbd_ref in zip(depths, rbd_ref_i):
    rbd= p11SoilData.getPileEndBearingCapacityAtDepth(depth, characteristic= False)
    err= (rbd-rbd_ref)/rbd_ref
    err_rbd+= err**2
err_rbd= math.sqrt(err_rbd)/len(rbd_ref_i)
testOK&= (err_rbd<1e-4)

# Check total bearing capacity.
## Characteristic values.
rck_ref_i= [1154.4e3, 1717.9e3, 2318.4e3, 2955.9e3, 3630.4e3, 7144.8e3, 8372.1e3, 9644.5e3, 10286.4e3, 10939.6e3, 12302.3e3, 13710.3e3, 15163.4e3, 16661.7e3, 18205.1e3, 19793.7e3, 21417.4e3, 23041e3, 24664.6e3, 25795.6e3, 26926.6e3, 28057.6e3, 29188.5e3, 30319.5e3, 31450.5e3, 32581.4e3, 33712.4e3, 34843.4e3, 35974.4e3, 37105.3e3, 38236.3e3, 39367.3e3, 40498.3e3, 41629.2e3, 42760.2e3, 43891.2e3, 45022.2e3, 46153.1e3]

err_rck= 0.0 # Root Mean Squared Relative Error
rck= 0.0
for depth, rck_ref in zip(depths, rck_ref_i):
    rck= p11SoilData.getPileBearingCapacityAtDepth(depth, characteristic= True)
    err= (rck-rck_ref)/rck_ref
    err_rck+= err**2
err_rck= math.sqrt(err_rck)/len(rck_ref_i)
testOK&= (err_rck<1e-4)

## Design values.
rcd_ref_i= [644.7e3, 981.2e3, 1341.6e3, 1726.2e3, 2134.7e3, 4077.1e3, 4814.8e3, 5581.8e3, 5969e3, 6363.5e3, 7189.1e3, 8044.2e3, 8928.5e3, 9842.2e3, 10785.1e3, 11757.5e3, 12752.5e3, 13747.6e3, 14742.7e3, 15477.1e3, 16211.5e3, 16945.9e3, 17680.3e3, 18414.7e3, 19149.1e3, 19883.5e3, 20617.9e3, 21352.3e3, 22086.7e3, 22821.1e3, 23555.5e3, 24289.8e3, 25024.2e3, 25758.6e3, 26493e3, 27227.4e3, 27961.8e3, 28696.2e3]

err_rcd= 0.0 # Root Mean Squared Relative Error
rcd= 0.0
for depth, rcd_ref in zip(depths, rcd_ref_i):
    rcd= p11SoilData.getPileBearingCapacityAtDepth(depth, characteristic= False)
    err= (rcd-rcd_ref)/rcd_ref
    err_rcd+= err**2
err_rcd= math.sqrt(err_rcd)/len(rcd_ref_i)
testOK&= (err_rcd<1e-4)

'''
print('Error in cohesion values; err_c= ', err_c)
print('Erron in internal friction angle values; err_phi= ', err_phi)
print('Error in characteristic skin friction bearing capacity; err_rsk= ', err_rsk)
print('Error in design skin friction bearing capacity; err_rsd= ', err_rsd)
print('Error in characteristic end bearing capacity; err_rbk= ', err_rbk)
print('Error in design end bearing capacity; err_rbd= ', err_rbd)
print('Error in characteristic bearing capacity; err_rck= ', err_rck)
print('Error in design bearing capacity; err_rcd= ', err_rcd)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
