# -*- coding: utf-8 -*-
'''Home cooked test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom

plg= geom.Polygon3d([geom.Pos3d(0,0,0), geom.Pos3d(1,0,0), geom.Pos3d(1,1,0), geom.Pos3d(0,1,0)])
center= plg.getCenterOfMass()

inside= plg.In(center,1e-4)

'''
print('center: ', center, 'inside: ', inside)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if inside:
    print("test "+fname+": ok.")
else:
    print("test "+fname+": ERROR.")
