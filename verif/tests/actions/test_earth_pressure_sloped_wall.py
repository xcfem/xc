# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
import xc
import math
from actions.earth_pressure import earth_pressure as ep


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

'''Home-made test on earth pressures on a sloped wall
'''
Ksoil=0.5    #pressure coefficient of the soil
gammaSoil=2  #weight density of the soil
zGroundPnt1=4 #global Z coordinate of ground level at point 1
xPnt1=1       # X coordinate of point 1
yPnt1=-3      # Y coordinate of point 1
zGroundPnt2=8 #global Z coordinate of ground level at point 2
xPnt2=6       # X coordinate of point 2
yPnt2=-5      # Y coordinate of point 2

soil=ep.EarthPressureSlopedWall(Ksoil=Ksoil,gammaSoil=gammaSoil,zGroundPnt1=zGroundPnt1,XYpnt1=(xPnt1,yPnt1),zGroundPnt2=zGroundPnt2,XYpnt2=(xPnt2,yPnt2))
z=1
pres1=soil.getPressure(xPnt1,yPnt1,z)
pres2=soil.getPressure(xPnt1+1/3.*(xPnt2-xPnt1),yPnt1+1/3.*(yPnt2-yPnt1),z)
pres3=soil.getPressure(xPnt1+1/2.*(xPnt2-xPnt1),yPnt1+1/2.*(yPnt2-yPnt1),z)
pres4=soil.getPressure(xPnt2,yPnt2,z)

pres1_tg=Ksoil*gammaSoil*(zGroundPnt1-z)
pres4_tg=Ksoil*gammaSoil*(zGroundPnt2-z)
pres2_tg=pres1_tg+1/3.*(pres4_tg-pres1_tg)
pres3_tg=pres1_tg+1/2.*(pres4_tg-pres1_tg)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(pres1-pres1_tg)<1e-6 and abs(pres2-pres2_tg)<1e-6 and abs(pres3-pres3_tg)<1e-6 and abs(pres4-pres4_tg)<1e-6:
    print("test ",fname,": ok.")
else:
    lmsg.error(fname+' ERROR.')
    
