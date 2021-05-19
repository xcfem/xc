# -*- coding: utf-8 -*-

import xc_base
import geom

def projXYPline3d(xyzPline):
  retval= geom.Polyline2d()
  for p in xyzPline:
    retval.appendVertex(geom.Pos2d(p.x,p.y))
  return retval

