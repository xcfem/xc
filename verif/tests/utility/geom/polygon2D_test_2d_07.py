# -*- coding: utf-8 -*-
from __future__ import print_function
import xc_base
import geom
pol1=geom.Polygon2d()
pol1.appendVertex(geom.Pos2d(0,0))
pol1.appendVertex(geom.Pos2d(1,0))
pol1.appendVertex(geom.Pos2d(1,1))
pol1.appendVertex(geom.Pos2d(0,1))

perimPol1=pol1.getPerimeter()

pol2=pol1.offset(-0.25)
perimPol2=pol2.getPerimeter()

ratio1= (perimPol1-4)/4.
ratio2= (perimPol2-2)/2.

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

