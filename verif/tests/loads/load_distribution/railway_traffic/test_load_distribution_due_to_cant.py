# -*- coding: utf-8 -*-
'''Loads on rails due to cant. Trivial test.'''


__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2023, A_OO LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import geom
from actions.railway_traffic import track_cross_section_geometry as tcs


h= 1.8
s= 1.435+80e-3

# No cant, no height, horizontal load.
[qlh, qlv, qrh, qrv]= tcs.get_rail_loads(Q= [1, 0, 0], h= 0.0, s= s, u= 0.0)

err0= math.sqrt((qlh-0.5)**2+qlv**2+(qrh-0.5)**2+qrv**2)

# No cant, vertical load.
[qlh, qlv, qrh, qrv]= tcs.get_rail_loads(Q= [0, 1, 0], h= h, s= s, u= 0.0)

err1= math.sqrt((qlv-0.5)**2+qlh**2+(qrv-0.5)**2+qrh**2)

# No height, vertical load.
[qlh, qlv, qrh, qrv]= tcs.get_rail_loads(Q= [0, 1, 0], h= 0.0, s= s, u= 0.15)
err2= math.sqrt((qlv-0.5)**2+qlh**2+(qrv-0.5)**2+qrh**2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err0<1e-5 and err1<1e-5 and err2<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
