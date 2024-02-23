# -*- coding: utf-8 -*-
''' Pyramidal frustum class.

A pyramidal frustum is a frustum made by chopping the top off a pyramid. It is a special case of a prismatoid. 
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom

class PyramidalFrustum(object):
    ''' Parabola

    :ivar h: height.
    :ivar bottomPolygon: bottom area.
    :ivar topPolygon: top area.
    '''
    def __init__(self, h, bottomPolygon, topPolygon):
        ''' Constructor.

        :param h: height.
        :param bottomPolygon: bottom area.
        :param topPolygon: top area.
        '''
        self.h= h
        self.bottomPolygon= bottomPolygon
        self.topPolygon= topPolygon

    def getVolume(self):
        ''' Return the frustum volume.'''
        A1= self.bottomPolygon.getArea()
        A2= self.topPolygon.getArea()
        return 1/3.0*self.h*(A1+A2+math.sqrt(A1*A2))

    def getGeometricCentroid(self):
        ''' Return the position of the geometric centroid of the frustum
            assuming that the z coordinate of the bottom face is zero
            and its height is vertical.'''    
        A1= self.bottomPolygon.getArea()
        A2= self.topPolygon.getArea()
        sqrtProd= math.sqrt(A1*A2)
        z= self.h*(A1+2*sqrtProd+3*A2)/(4*(A1+sqrtProd+A2))
        a1Centroid= self.bottomPolygon.getCenterOfMass()
        a1Centroid= geom.Pos3d(a1Centroid.x, a1Centroid.y, 0.0)
        a2Centroid= self.topPolygon.getCenterOfMass()
        a2Centroid= geom.Pos3d(a2Centroid.x, a2Centroid.y, self.h)
        axis= geom.Segment3d(a1Centroid, a2Centroid)
        o= geom.Pos3d(0,0,z)
        p1= geom.Pos3d(1,0,z)
        p2= geom.Pos3d(0,1,z)
        cutPlane=geom.Plane3d(o,p1,p2)
        retval= cutPlane.getIntersection(axis)
        return retval

