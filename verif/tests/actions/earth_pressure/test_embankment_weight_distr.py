# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es" "l.pereztato@ciccp.es" 

import math
from actions.earth_pressure import earth_pressure as ep

gammaSoil=2  #weight density of the soil

coordSoilSurf=[[0,0],
                  [1,0.5],
                  [3,3],
                  [4,0]] #list of coordinates [[yp1,z1],[yp2,z2], ...] that defines the soil surface, where yp are the y coordinates of the vertices of a transversal section in the surface, expressed in the rotated reference system.

theta= math.radians(0) # angle counterclockwise in radians that forms the axis of the strip of embankment with the global X-axis
soil0=ep.WeightDistrEmbankment(gammaSoil=gammaSoil,theta=theta,coordSoilSurf=coordSoilSurf)
theta=90
soil90=ep.WeightDistrEmbankment(gammaSoil=gammaSoil,theta=theta,coordSoilSurf=coordSoilSurf)
theta=45
soil45=ep.WeightDistrEmbankment(gammaSoil=gammaSoil,theta=theta,coordSoilSurf=coordSoilSurf)

#strip parallel to Y-axis
x0=[0.5,2,3.5]
y=10
z=0
pres0=[soil0.getPressure(x0[i],y,z) for i in range(len(x0))]

#strip parallel to X-axis
y90=[0.5,2,3.5]
x=10
z=0
pres90=[soil90.getPressure(x,y90[i],z) for i in range(len(y90))]

cos45=math.cos(math.radians(45))
x45=[0.5*cos45,2*cos45,3.5*cos45]
pres45=[soil45.getPressure(x45[i],x45[i],z) for i in range(len(x45))]

presComp=[0.5,3.5,3.0]

OK01= abs(pres0[0]-presComp[0])<1e-6 and abs(pres0[1]-presComp[1])<1e-6 and abs(pres0[2]-presComp[2])<1e-6 and abs(pres90[0]-presComp[0])<1e-6 and abs(pres90[1]-presComp[1])<1e-6 and abs(pres90[2]-presComp[2])<1e-6 and abs(pres45[0]-presComp[0])<1e-6 and abs(pres45[1]-presComp[1])<1e-6 and abs(pres45[2]-presComp[2])<1e-6

'''
print(OK01)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if OK01:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

