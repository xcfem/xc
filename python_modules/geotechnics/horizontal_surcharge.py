# -*- coding: utf-8 -*-
''' Horizontal surcharge on backfill surfacce.

   Code based on the procedure detailed in:
   https://estructurando.net/2017/04/18/empujes-muros-sobrecarga-horizontal/
'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from misc_utils import log_messages as lmsg

class HorizontalLoadOnBackfill3D(object):
    ''' Horizontal surcharge on backfill surface. This load can be used 
        on general 3D problems.

    :ivar contour: contour vertices.
    :ivar H: horizontal load vector.
    '''
    def __init__(self, contour, H):
        ''' Constructor.

        :param contour: vertices of the 3D polygon defining the contour of
                        the loaded area.
        :param H: horizontal load vector.
        '''
        self.contour= contour
        self.H= H

    def getPolygon(self):
        ''' Return the polygon whose contour are the those of the
            loaded area.'''
        return geom.Polygon3d(self.contour)

    def getCentroid(self):
        ''' Return the position of the load centroid.'''
        return self.getPolygon().getCenterOfMass()

    def getLoadedArea(self, loadedPlane, phi):
        ''' Return the polygon on the loadedPlane affected by this horizontal
        load.

        :param loadedPlane: plane under load.
        :param soil: effective friction angle of soil.
        '''
        retval= None
        if(len(self.contour)>0):
            # Compute nearest and farthest vertices.
            nearestVertex= self.contour[0]
            nearestVertexDist= loadedPlane.dist(nearestVertex)
            farthestVertex= self.contour[0]
            farthestVertexDist= loadedPlane.dist(farthestVertex)
            vertexDistances= list() # Distances from the vertices to the plane.
            for v in self.contour:
                dist= loadedPlane.dist(v)
                vertexDistances.append(dist)
                if(dist<nearestVertexDist):
                    nearestVertexDist= dist
                    nearestVertex= v
                if(dist>farthestVertexDist):
                    farthestVertexDist= dist
                    farthestVertex= v
            # Compute vertical angles.
            pi_4= math.pi/4.0
            xi= pi_4+phi/2
            vertexVerticalAngles= list() # Vertical angle of the ray for each vertex.
            for v, dist in zip(self.contour, vertexDistances):
                lmbd= (dist-nearestVertexDist)/(farthestVertexDist-nearestVertexDist)
                verticalAngle= lmbd*(xi-phi)+phi
                vertexVerticalAngles.append(verticalAngle)
            # Compute horizontal angles.
            centroid= self.getCentroid()
            ## Ray in the direction of the load.
            loadRay= geom.Ray3d(centroid, 100.0*self.H)
            rayIntersection= loadedPlane.getIntersection(loadRay)
            if(rayIntersection): # no intersection -> no loads on plane.
                verticalPlane= geom.Plane3d(rayIntersection, centroid, centroid+geom.Vector3d(0,0,100.0))
                vertexHorizontalAngles= list()
                for v in self.contour:
                    positiveSide= verticalPlane.positiveSide(v)
                    if(positiveSide):
                        vertexHorizontalAngles.append(pi_4)
                    else:
                        vertexHorizontalAngles.append(-pi_4)
                # Compute intersections of the rays with the loaded plane.
                loadShadowContour= list()
                baseDir= self.H.normalized() # Direction of the load.
                for v, vAngle, hAngle in zip(self.contour, vertexVerticalAngles, vertexHorizontalAngles):
                    # Compute rotation.
                    ## Horizontal axis.
                    horizLine= geom.Line3d(v, 100.0*verticalPlane.getNormal())
                    vertRotation= geom.Rotation3d(horizLine, vAngle)
                    ## Vertical axis.
                    vertLine= geom.Line3d(v,geom.Vector3d(0,0,100.0))
                    horizRotation= geom.Rotation3d(vertLine, hAngle)
                    ## Composed rotatation
                    rotation= vertRotation*horizRotation
                    rotatedVector= rotation.getTrfVector3d(baseDir)
                    ## Compute vertex "shadow"
                    lightRay= geom.Ray3d(v, 100.0*rotatedVector)
                    vertexShadow= loadedPlane.getIntersection(lightRay)
                    if(vertexShadow):
                        loadShadowContour.append(vertexShadow)
                    else:
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; error computing load "shadow".')
            retval= geom.Polygon3d(loadShadowContour)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; contour list is empty.')
        return retval
            
