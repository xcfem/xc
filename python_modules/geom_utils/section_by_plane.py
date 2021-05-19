# -*- coding: utf-8 -*-

import xc_base
import geom

class SectionByPlane(object):
  '''Intersection of a polyline or a list of polylines by a plane.'''
  name= "-"
  plane= geom.Plane3d(geom.Pos3d(1,0,0), geom.Pos3d(0,1,0), geom.Pos3d(0,0,1))

  def __init__(self,nm,p):
    self.name= nm
    self.plane= p

  def getIntersection(self,pline):
    '''Returns the intersection of the polyline with the plane.'''
    return pline.getIntersection(self.plane)

  def getIntersectionPoints(self,listOfPlines):
    '''Returns the intersection of a list of polylines with the plane.'''
    retval= geom.Polyline3d()
    for pline in listOfPlines:
      lp= pline.getIntersection(self.plane)
      retval.appendVertex(lp[0])
    return retval
