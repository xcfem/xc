# -*- coding: utf-8 -*-
''' Trivial test tesf of the PyramidalFrustum object.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geom_utils import pyramidal_frustum as pf

p0= geom.Pos2d(0,0)
p1= geom.Pos2d(1.0,0.0)
p2= geom.Pos2d(1.0,1.0)
p3= geom.Pos2d(0.0,1.0)

bottomFace= geom.Polygon2d([p0, p1, p2, p3])
topFace= bottomFace
pyramidalFrustum= pf.PyramidalFrustum(h= 1.0, bottomPolygon= bottomFace, topPolygon= topFace)

volume= pyramidalFrustum.getVolume()
ratio1= abs(volume-1.0)
centroid= pyramidalFrustum.getGeometricCentroid()
ratio2= centroid.dist(geom.Pos3d(0.5, 0.5, 0.5))

'''
print('volume: ', volume, ratio1)
print('centroid: ', centroid, ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-8) and (ratio2<1e-8):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
