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

import sys
import math
import geom
import xc
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
        # Check if the point is on the plane (or almost)
        dist= loadedPlane.dist(v)
        if(dist<1e-6): # The point is almost on the plane, take the point as intersection
            retval= v
        else:
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
            retval= loadedPlane.getIntersection(lightRay)
            if(not retval.exists):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; horizontal load not pointing towards the loaded plane. Horizontal load: '+str(self.H)+ ' loaded plane: '+str(loadedPlane))
                retval= None
        return retval
    
    def computeElementOrientation(self, elements):
        ''' Compute the orientation of the elements with respect to this
        load.

        :param elements: elements to compute the orientation of.
        '''
        # Get the centroid of the load.
        loadCentroid= self.getCentroid()
        # Get loaded points.
        loadedPoints= list()
        loadedAreas= list()
        unitVectors= list()
        for e in elements:
            pos= e.getPosCentroid(True)
            loadedPoints.append(pos) # Append loaded point.
            loadedAreas.append(e.getArea(True))
            # Compute normal vector 
            kVector= e.getKVector3d(True)
            orientation= kVector.dot(loadCentroid-pos)
            if(orientation>0.0): # pressure on the "positive" side of the element.
                unitVectors.append(kVector)
            else: # pressure on the "negative" side of the element.
                unitVectors.append(-kVector)
        return loadedPoints, loadedAreas, unitVectors

    def getForceDistribution(self, points, areas, phi):
        ''' Return the distribution of the forces over the points.

        :param points: points whose stress increment will be computed.
        :param areas: tributary areas corresponding to the points.
        :param phi: effective friction angle of soil.
        '''
        forceVectors= list()
        loadedAreaRatio= 0.0
        # Compute the area subject to pressure.
        loadedPlane= geom.Plane3d(points) # compute regression plane.
        shadowContour= self.getLoadedArea(loadedPlane, phi= phi)
        if(shadowContour):
            shadowContourArea= shadowContour.getArea()
            loadedArea= 0.0
            loadedAreas= list()
            numberOfLoadedPoints= 0
            for p, area, in zip(points, areas):
                if(shadowContour.In(p, 1e-2)): # if point inside loaded area.
                    loadedArea+= area
                    loadedAreas.append(area)
                    numberOfLoadedPoints+= 1
                else: # ouside loaded area
                    loadedAreas.append(0.0) # area= 0 => no load on this point.
            loadedAreaRatio= loadedArea/shadowContourArea
            # Compute the load distribution.
            shadowContourCentroid= shadowContour.getCenterOfMass()
            F= loadedAreaRatio*self.H # Load to distribute over the points.
            if(numberOfLoadedPoints>1): # isostatic distribution over the points. 
                verticalSegment= shadowContour.clip(geom.Line3d(shadowContourCentroid, geom.Vector3d(0,0,100)))
                ## Get a point 1/3*length down from the top point.
                fromPoint= verticalSegment.getFromPoint()
                segmentTopPoint= verticalSegment.getToPoint()
                if(segmentTopPoint.z<fromPoint.z):
                    segmentTopPoint= fromPoint
                loadApplicationPoint= segmentTopPoint+(1/3)*verticalSegment.getLength()*geom.Vector3d(0,0,-1)
                ## Create the sliding vector system.
                M= geom.Vector3d(0,0,0) # Zero moment at this point.
                loadSVS= geom.SlidingVectorsSystem3d(loadApplicationPoint,F,M)
                forceVectors= loadSVS.distribute(points, loadedAreas)
            else: # no distribution needed.
                forceVectors= list()
                for p, a in zip(points, loadedAreas):
                    M= geom.Vector3d(0,0,0) # Zero moment at this point.
                    if(a>0.0): # Concentrated load.
                        forceVector= geom.SlidingVectorsSystem3d(p,F,M)
                    else: # Zero load.
                        forceVector= geom.SlidingVectorsSystem3d(p,M,M)
                    forceVectors.append(forceVector)
        return forceVectors, loadedAreaRatio
        
    def computeElementalLoads(self, elements, forceVectors, delta):
        ''' Compute loads on elements from the stress vectors passes as
            parameter.

        :param elements: elements to compute the pressure on.
        :param forceVectors: value of the stress vector (cartesian coord.)
                             on the centroid of each element.
        :param delta: friction angle between the soil and the element material.
        '''
        tanDelta= math.tan(delta)
        retval= list()
        for e, forceVector in zip(elements, forceVectors):
            if(forceVector.getModulus()>1e-6):
                iVector= e.getIVector3d(True)
                jVector= e.getJVector3d(True)
                kVector= e.getKVector3d(True)
                # Normal pressure.
                stressVector= forceVector.getVector3d()*(1.0/e.getArea(True))
                normalPressure= stressVector.dot(kVector)
                maxTangentPressure= abs(tanDelta*normalPressure)
                # Pressure parallel to i vector.
                tangentIPressure= stressVector.dot(iVector)
                if(abs(tangentIPressure)>maxTangentPressure):
                    tangentIPressure= math.copysign(maxTangentPressure, tangentIPressure)
                # Pressure parallel to j vector.
                tangentJPressure= stressVector.dot(jVector)
                if(abs(tangentJPressure)>maxTangentPressure):
                    tangentJPressure= math.copysign(maxTangentPressure, tangentJPressure)
                # Append the computed values to the returned list.
                retval.append([e, tangentIPressure, tangentJPressure, normalPressure])
            else: # No load in this element.
                retval.append([e, 0.0, 0.0, 0.0])
        return retval
    
    def computeForcesOnElements(self, elements, phi, delta= 0.0):
        ''' Compute pressures due to this load on the elements argument.

        :param elements: elements to compute the pressure on.
        :param phi: effective friction angle of soil.
        :param delta: friction angle between the soil and the element material.
        '''
        elementalLoads= list()
        loadedAreaRatio= 0.0
        if(self.H.getModulus()>1e-6): # If load not zero.
            # Compute element orientation with respect to this load.
            loadedPoints, loadedAreas, unitVectors= self.computeElementOrientation(elements= elements)
            # Compute the force distribution.
            forceVectors, loadedAreaRatio= self.getForceDistribution(points= loadedPoints, areas= loadedAreas, phi= phi)
            # Compute loads on elements.
            elementalLoads= self.computeElementalLoads(elements= elements, forceVectors= forceVectors, delta= delta)
        return elementalLoads, loadedAreaRatio
    
    def appendLoadToCurrentLoadPattern(self, elements, phi, delta= 0.0):
        ''' Append this load to the current load pattern.

        :param elements: elements to apply the load on.
        :param phi: effective friction angle of soil.
        :param delta: friction angle between the soil and the element material.
        '''
        pressures, loadedAreaRatio= self.computeForcesOnElements(elements= elements, phi= phi, delta= delta)
        for pData in pressures:
            if(pData[3]!=0.0): # if actually loaded.
               e= pData[0]
               e.vector3dUniformLoadLocal(xc.Vector([pData[1],pData[2],pData[3]]))
        return loadedAreaRatio

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

    def getCentroid(self):
        ''' Return the centroid of the loaded area.'''
        return self.pos

    def getLoadedArea(self, loadedPlane, phi):
        ''' Return the polygon on the loadedPlane affected by this horizontal
        load.

        :param loadedPlane: plane under load.
        :param phi: effective friction angle of soil.
        '''
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
        if(rayIntersection.exists): # no intersection -> no loads on plane.
            verticalPlane= geom.Plane3d(rayIntersection, self.pos, self.pos+geom.Vector3d(0,0,100.0))
            # Compute horizontal angles.
            horizontalAngles= [pi_4, -pi_4]
            # Compute intersections of the rays with the loaded plane.
            loadShadowContour= list()
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[0], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[1], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[1], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            loadShadowContour.append(self.getVertexShadow(v= self.pos, hAngle= horizontalAngles[0], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
            for vertexShadow in loadShadowContour:
                if(vertexShadow is None):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; error computing load "shadow".')
                    exit(1)
            retval= geom.Polygon3d(loadShadowContour)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; horizontal load not pointing towards the loaded plane.')
            retval= None
        return retval

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
            if(rayIntersection.exists): # no intersection -> no loads on plane.
                verticalPlane= geom.Plane3d(rayIntersection, centroid, centroid+geom.Vector3d(0,0,100.0))
                # Compute horizontal angles.
                # Compute horizontal angles.
                horizontalAngles= [pi_4, -pi_4]
                # Compute intersections of the rays with the loaded plane.
                loadShadowContour= list()
                loadShadowContour.append(self.getVertexShadow(v= nearestVertex, hAngle= horizontalAngles[0], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= nearestVertex, hAngle= horizontalAngles[1], vAngle= verticalAngles[0], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= farthestVertex, hAngle= horizontalAngles[1], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                loadShadowContour.append(self.getVertexShadow(v= farthestVertex, hAngle= horizontalAngles[0], vAngle= verticalAngles[1], verticalPlane= verticalPlane, loadedPlane= loadedPlane))
                for vertexShadow in loadShadowContour:
                    if(vertexShadow is None):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; error computing load "shadow".')
                        exit(1)
                retval= geom.Polygon3d(loadShadowContour)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; horizontal load not pointing towards the loaded plane.')
                retval= None
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
            if(rayIntersection.exists): # intersection found.
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
                        exit(1)
                retval= geom.Polygon3d(loadShadowContour)
            else: # no intersection -> no loads on plane.
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; horizontal load not pointing towards the loaded plane.')                
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; contour list is empty.')
        return retval
            
