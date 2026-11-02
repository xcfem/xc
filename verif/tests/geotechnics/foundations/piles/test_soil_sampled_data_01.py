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

codes= pgpSet.getCodes()

khi= list()
khi_ref= [107849000.0, 26667000.0, 12444000.0, 14222000.0, 14222000.0, 14222000.0, 14222000.0, 22222000.0, 14222000.0, 14222000.0, 17778000.0, 17778000.0, 17778000.0, 17778000.0, 17778000.0, 24444000.0, 17778000.0, 17778000.0, 17778000.0, 12444000.0, 17778000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0, 44444000.0]
phii= list()
phii_ref= [None, None, 0.29670597283903605, 0.29670597283903605, 0.29670597283903605, 0.29670597283903605, 0.29670597283903605, 0.4188790204786391, 0.29670597283903605, 0.29670597283903605, 0.4188790204786391, 0.3272492347489368, 0.3272492347489368, 0.4188790204786391, 0.3577924966588376, 0.4188790204786391, 0.4188790204786391, 0.4188790204786391, 0.4188790204786391, 0.29670597283903605, 0.4188790204786391, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897, 0.47123889803846897]
ci= list()
ci_ref= [None, None, 20000.0, 20000.0, 20000.0, 20000.0, 20000.0, 35000.0, 20000.0, 20000.0, 35000.0, 23750.0, 23750.0, 35000.0, 27500.0, 35000.0, 35000.0, 35000.0, 35000.0, 20000.0, 35000.0, 45000.0, 45000.0, 45000.0, 45000.0, 45000.0, 45000.0, 45000.0, 45000.0, 45000.0]
gammai= list()
gammai_ref= [26250.0, 26250.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 19890.0, 21060.0, 21060.0, 21060.0, 21060.0, 21060.0, 21060.0, 21060.0, 21060.0, 21060.0]
rcki= list()
rcki_ref= [30861600.000000034, 30861600.000000034, 4529025.0, 4529025.0, 4529025.0, 4529025.0, 4529025.0, 7451625.0, 4529025.0, 4529025.0, 7451625.0, 5293750.0, 5293750.0, 8146425.0, 6110850.0, 8346550.0, 8066550.0, 7933800.0, 7724225.0, 4482800.0, 7922525.000000001, 11333675.0, 11333675.0, 11953300.0, 11953300.0, 11944400.0, 11953300.0, 11953300.0, 11953300.0, 11953300.0]
qui= list()
qui_ref= [40220000.0, 40220000.0, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None]
rqdi= list()
rqdi_ref= [50.0, 50.0, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None]

depth= 12.5 # m
for code in codes:
    pileGP= pgpSet.getSoilSampledData(code)
    kh= pileGP.getKhAtDepth(depth)
    khi.append(kh)
    phi= pileGP.getInternalFrictionAngleAtDepth(depth)
    phii.append(phi)
    c= pileGP.getCohesionAtDepth(depth)
    ci.append(c)
    gamma= pileGP.getUnitWeightAtDepth(depth)
    gammai.append(gamma)
    rck= pileGP.getPileBearingCapacityAtDepth(depth, characteristic= True)
    rcki.append(rck)
    qu= pileGP.getQuAtDepth(depth)
    qui.append(qu)
    rqd= pileGP.getRockQualityDesignationAtDepth(depth)
    rqdi.append(rqd)
    # pileGP.plotInternalFrictionAngleAlongDepth()
    # pileGP.plotCohesionAlongDepth()
    # pileGP.plotUnitWeightAlongDepth()
    # pileGP.plotPileBearingCapacityAlongDepth(characteristic= True)
    # pileGP.plotPileBearingCapacityAlongDepth(characteristic= False)
    # pileGP.plotQuAlongDepth()
    # pileGP.plotRockQualityDesignationAlongDepth()
    # pileGP.plotKhAlongDepth()

err= 0.0
for kh, kh_ref in zip(khi, khi_ref):
    err+= (kh-kh_ref)**2
for phi, phi_ref in zip(phii, phii_ref):
    if(phi is None):
        if(phi_ref is not None):
            err+= 100.0
    else:
        err+= (phi-phi_ref)**2
for c, c_ref in zip(ci, ci_ref):
    if(c is None):
        if(c_ref is not None):
            err+= 100.0
    else:
        err+= (c-c_ref)**2
for gamma, gamma_ref in zip(gammai, gammai_ref):
    err+= (gamma-gamma_ref)**2
for rck, rck_ref in zip(rcki, rcki_ref):
    err+= (rck-rck_ref)**2
for qu, qu_ref in zip(qui, qui_ref):
    if(qu is None):
        if(qu_ref is not None):
            err+= 100.0
    else:
        err+= (qu-qu_ref)**2
for rqd, rqd_ref in zip(rqdi, rqdi_ref):
    if(rqd is None):
        if(rqd_ref is not None):
            err+= 100.0
    else:
        err+= (rqd-rqd_ref)**2
    
err= math.sqrt(err)

# print(err)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-8:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
