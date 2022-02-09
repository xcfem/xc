# -*- coding: utf-8 -*-
'''Compute the intersection of a line with a segment.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import math

sg= geom.Segment2d(geom.Pos2d(-0.443,0.443),geom.Pos2d(-0.443,-0.443))
sr= geom.Ray2d(geom.Pos2d(0.19855,-0.082042),geom.Pos2d(-39.7765,16.6642))
p= sg.getIntersection(sr)[0] 

d= sr.dist(p)

#print('d2= ', d)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(d<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
