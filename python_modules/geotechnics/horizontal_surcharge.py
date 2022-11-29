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

class HorizontalLoadOnBackFill(object):
    ''' Base class for horizontal loads on backfill surface.

    :ivar H: horizontal load vector.
    '''
    def __init__(self, H):
        ''' Constructor.

        :param H: horizontal load vector.
        '''
        self.H= H

    def getVertexShadow(self, v, hAngle, vAngle, verticalPlane, loadedPlane):
        ''' Compute the intersection of a ray defined by the vertex argument
        and the angles with respect the horizontal plane and the vertical
        plane argument with the loaded plane.
        '''
        baseDir= self.H.normalized() # Direction of the load.
        # Compute rotation.
        ## Horizontal axis.
        horizLine= geom.Line3d(v, 100.0*verticalPlane.getNormal())
        vertRotation= geom.Rotation3d(horizLine, vAngle)
        ## Vertical axis.
        vertLine= geom.Line3d(v,geom.Vector3d(0,0,100.0))
        horizRotation= geom.Rotation3d(vertLine, hAngle)
        ## Composed rotatation (WARNING: not commutative).
        rotation= horizRotation*vertRotation
        rotatedVector= rotation.getTrfVector3d(baseDir)
        ## Compute vertex "shadow"
        lightRay= geom.Ray3d(v, 100.0*rotatedVector)
        return loadedPlane.getIntersection(lightRay)

class HorizontalConcentratedLoadOnBackfill3D(HorizontalLoadOnBackFill):
    ''' Horizontal concentrated surcharge on backfill surface. This load 
        can be used on general 3D problems.

    :ivar pos: load position.
    '''
    def __init__(self, pos, H):
        ''' Constructor.

        :param pos: load position.
        :param H: horizontal load vector.
        '''
        super().__init__(H= H)
        self.pos= pos

    def getLoadedArea(self, loadedPlane, phi):
        ''' Return the polygon on the loadedPlane affected by this horizontal
        load.

        :param loadedPlane: plane under load.
        :param soil: effective friction angle of soil.
        '''
        retval= None
        # Compute vertical angles.
        pi_4= math.pi/4.0
        horizontalAngle= pi_4
        # The vertical angle depends on the horizontal rotation:
        rotatedVerticalAngleUp= math.atan(math.tan(phi)*math.sin(horizontalAngle))
        xi= pi_4+phi/2
        rotatedVerticalAngleDown= math.atan(math.tan(xi)*math.sin(horizontalAngle))
        verticalAngles= [rotatedVerticalAngleUp, rotatedVerticalAngleDown]
        # Ray in the direction of the load.
        loadRay= geom.Ray3d(self.pos, 100.0*self.H)
        rayIntersection= loadedPlane.getIntersection(loadRay)
        if(rayIntersection): # no intersection -> no loads on plane.
            verticalPlane= geom.Plane3d(rayIntersection, self.pos, self.pos+geom.Vector3d(0,0,100.0))
            # Compute horizontal angles.
            horizontalAngles= [pi_4, -pi_4]
            # Compute intersections of the rays with the loaded plane.
            loadShadowContour= list()
            baseDir= self.H.normalized() # Direction of the load.
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[0], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[1], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[1], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[0], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            for vertexShadow in loadShadowContour:
                if(vertexShadow is None):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; error computing load "shadow".')
        return geom.Polygon3d(loadShadowContour)

class HorizontalLinearLoadOnBackfill3D(HorizontalLoadOnBackFill):
    ''' Horizontal linear surcharge on backfill surface. This load 
        can be used on general 3D problems.

    :ivar endPoints: end points of the line.
    '''
    def __init__(self, endPoints, H):
        ''' Constructor.

        :param endPoints: end points of the line.
        :param H: horizontal load vector.
        '''
        super().__init__(H= H)
        self.endPoints= endPoints

    def getSegment(self):
        ''' Return the segment between polygon whose contour are the those of the
            loaded area.'''
        return geom.Segment3d(self.endPoints[0], self.endPoints[1])
    
    def getCentroid(self):
        ''' Return the position of the load centroid.'''
        return self.getSegment().getCenterOfMass()
    
    def getLoadedArea(self, loadedPlane, phi):
        ''' Return the polygon on the loadedPlane affected by this horizontal
        load.

        :param loadedPlane: plane under load.
        :param soil: effective friction angle of soil.
        '''
        retval= None
        if(len(self.endPoints)>0):
            # Compute nearest and farthest vertices.
            dist0= loadedPlane.dist(self.endPoints[0])
            dist1= loadedPlane.dist(self.endPoints[1])
            if(dist0<dist1):
                nearestVertex= self.endPoints[0]
                farthestVertex= self.endPoints[1]
            else:
                nearestVertex= self.endPoints[1]
                farthestVertex= self.endPoints[0]
            # Compute vertical angles.
            pi_4= math.pi/4.0
            xi= pi_4+phi/2
            vertexVerticalAngles= list() # Vertical angle of the ray for each vertex.
            horizontalAngle= pi_4
            # The vertical angle depends on the horizontal rotation:
            rotatedVerticalAngleUp= math.atan(math.tan(phi)*math.sin(horizontalAngle))
            xi= pi_4+phi/2
            rotatedVerticalAngleDown= math.atan(math.tan(xi)*math.sin(horizontalAngle))
            verticalAngles= [rotatedVerticalAngleUp, rotatedVerticalAngleDown]
            
            centroid= self.getCentroid()
            # Ray in the direction of the load.
            loadRay= geom.Ray3d(centroid, 100.0*self.H)
            rayIntersection= loadedPlane.getIntersection(loadRay)
            if(rayIntersection): # no intersection -> no loads on plane.
                verticalPlane= geom.Plane3d(rayIntersection, centroid, centroid+geom.Vector3d(0,0,100.0))
                # Compute horizontal angles.
                # Compute horizontal angles.
                horizontalAngles= [pi_4, -pi_4]
                # Compute intersections of the rays with the loaded plane.
                loadShadowContour= list()
                baseDir= self.H.normalized() # Direction of the load.
                loadShadowContour.append(self.getVertexShadow(v= nearestVertex, hAngle= horizontalAngles[0], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= nearestVertex, hAngle= horizontalAngles[1], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= farthestVertex, hAngle= horizontalAngles[1], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= farthestVertex, hAngle= horizontalAngles[0], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                for vertexShadow in loadShadowContour:
                    if(vertexShadow is None):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; error computing load "shadow".')
            retval= geom.Polygon3d(loadShadowContour)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; contour list is empty.')
        return retval
        
class HorizontalLoadedAreaOnBackfill3D(HorizontalLoadOnBackFill):
    ''' Horizontal surcharge on backfill surface. This load can be used 
        on general 3D problems.

    :ivar contour: contour vertices.
    '''
    def __init__(self, contour, H):
        ''' Constructor.

        :param contour: vertices of the 3D polygon defining the contour of
                        the loaded area.
        :param H: horizontal load vector.
        '''
        super().__init__(H= H)
        self.contour= contour

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
            horizontalAngle= pi_4
            for v, dist in zip(self.contour, vertexDistances):
                lmbd= (dist-nearestVertexDist)/(farthestVertexDist-nearestVertexDist)
                verticalAngle= lmbd*(xi-phi)+phi
                # The vertical angle depends on the horizontal rotation:
                rotatedVerticalAngle= math.atan(math.tan(verticalAngle)*math.sin(horizontalAngle))
                vertexVerticalAngles.append(rotatedVerticalAngle)
            centroid= self.getCentroid()
            # Ray in the direction of the load.
            loadRay= geom.Ray3d(centroid, 100.0*self.H)
            rayIntersection= loadedPlane.getIntersection(loadRay)
            if(rayIntersection): # no intersection -> no loads on plane.
                verticalPlane= geom.Plane3d(rayIntersection, centroid, centroid+geom.Vector3d(0,0,100.0))
                # Compute horizontal angles.
                vertexHorizontalAngles= list()
                for v in self.contour:
                    positiveSide= verticalPlane.positiveSide(v)
                    if(positiveSide):
                        vertexHorizontalAngles.append(pi_4)
                    else:
                        vertexHorizontalAngles.append(-pi_4)
                # Compute intersections of the rays with the loaded plane.
                loadShadowContour= list()
                for v, vAngle, hAngle in zip(self.contour, vertexVerticalAngles, vertexHorizontalAngles):
                    vertexShadow= self.getVertexShadow(v= v, hAngle= hAngle, vAngle= vAngle, verticalPlane= verticalPlane, loadedPlane= loadedPlane)
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
            
