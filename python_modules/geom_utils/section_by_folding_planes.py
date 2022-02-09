# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import math

class XYFoldingPlanes(object):
    xyPline= None

    def __init__(self,xyPolyline):
        self.xyPline= xyPolyline
    def getIntersectionWith3DLine(self,p0, p1):
        retval= []
        err= 0.0
        P0proj= geom.Pos2d(p0.x,p0.y)
        P1proj= geom.Pos2d(p1.x,p1.y)
        line2d= geom.Line2d(P0proj,P1proj)
        proj= self.xyPline.getIntersection(line2d)
        for p in proj:
            lmb= p.dist(P0proj)/P1proj.dist(P0proj)
            pInt= geom.LineSegment3d(p0,p1).getPoint(lmb)
            err= (pInt.x-p.x)**2+(pInt.y-p.y)**2
            if(err>1e-6):
                print("Error finding intersection; err= ", err)
                print("p= ", p)
                print("pInt= ", pInt)
                print("lmb= ", lmb)
            else:
                retval.append(pInt)
        return retval

    def getIntersectionWith3DSegment(self,segment3d):
        retval= []
        p0= segment3d.getFromPoint()
        p1= segment3d.getToPoint()
        P0proj= geom.Pos2d(p0.x,p0.y)
        P1proj= geom.Pos2d(p1.x,p1.y)
        segment2d= geom.Segment2d(P0proj,P1proj)
        proj= self.xyPline.getIntersection(segment2d)
        for p in proj:
            lmb= p.dist(P0proj)/P1proj.dist(P0proj)
            pInt= segment3d.getPoint(lmb)
            retval.append(pInt)
        return retval  


