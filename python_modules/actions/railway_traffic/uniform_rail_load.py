# -*- coding: utf-8 -*-
''' Uniform load along a rail.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2023,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import sys
import geom
import xc
from actions import loads
from misc_utils import log_messages as lmsg
from actions.railway_traffic import dynamic_factor_load as dfl
from actions.railway_traffic import track_axis as ta

from geotechnics import horizontal_surcharge as hs
from geotechnics import boussinesq

# Railway traffic loads.
#     ____
#     |DD|____T_
#     |_ |_____|<
#     @-@-@-oo\
#
class RailLoadBase(dfl.DynamicFactorLoad):
    ''' Base class for loads along a rail.

    :ivar railAxis: 3D polyline defining the axis of the rail.
    '''
    def __init__(self, railAxis, dynamicFactor= 1.0, classificationFactor= 1.21):
        ''' Constructor.

        :param railAxis: 3D polyline defining the axis of the rail.
        :param directionVector: unitary vector in the direction of the load.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        super().__init__(dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.railAxis= railAxis

    def getMidpoint(self):
        ''' Return the midpoint of the rail axis.'''
        return self.railAxis.getPointAtLength(0.5*self.railAxis.getLength())

    def getClassifiedLoad(self):
        ''' Return the value of the load affected by the classification
            factor.'''
        return self.load*self.classificationFactor

    def getDynamicLoad(self):
        ''' Return the value of the load affected by the dynamic factor.'''
        return self.getClassifiedLoad()*self.dynamicFactor

    def clip(self, clippingPlane):
        ''' Clips the rail axis with the plane argument. It's used to remove
        the part of the rail axis that lies behind the plane (normally the
        plane will correspond to the midplane of a wall that will resist
        the pressures caused by this load. That way, if some of the rail axis
        lies behind the wall we'll remove the "negative" pressures caused
        by that chunk of the rail load.

        :param clippingPlane: plane to clip with.
        '''
        intersections= self.railAxis.getIntersection(clippingPlane)
        if(len(intersections)>0):
            if(len(intersections)>1):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+'; not implemented for multiple intersections.')
            else:
                pInt= intersections[0]
                chunks= self.railAxis.split(pInt)
                l0= chunks[0].getLength()
                l1= chunks[1].getLength()
                if(l0>l1):
                    self.railAxis= chunks[0]
                else:
                    self.railAxis= chunks[1]

    def getRailAxisProjection(self, midplane):
        ''' Return the projection of the rail axis onto the plane argument.

        :param midplane: mid-plane of the loaded surface.
        '''
        posList= list()
        ref= midplane.getRef()
        for v in self.railAxis.getVertexList():
            # vertLine= geom.Line3d(v, geom.Vector3d(0,0,-100.0))
            projPos= midplane.getProjection(v)
            posList.append(ref.getLocalPosition(projPos))
        return geom.Polyline2d(posList)

    def getLoadedContour(self, midplane, spreadingLayers= None):
        ''' Return the loaded contour of the wheel taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface represented by the plane argument.

        :param midplane: mid-plane of the loaded surface.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        '''
        # Compute spread.
        spread= 0.0
        if(spreadingLayers):
            for sl in spreadingLayers:
                layerDepth= sl[0]
                layerSpreadToDepthRatio= sl[1]
                spread+= layerDepth*layerSpreadToDepthRatio
        railProjection= self.getRailAxisProjection(midplane)
        rLength= railProjection.getLength()
        # Remove consecutive vertices that are too close
        railProjection.removeRepeatedVertexes(1e-3)
        # Remove backward segments.
        railProjection.removeBackwardSegments(-0.1)
        # Remove collinear contiguous segments from the polyline
        # otherwise there are vertexes that are not in a "corner"
        # so the buffer algorithm cannot found the interseccion
        # between the "offseted" lines.
        epsilon= railProjection.getLength()/1e3 # compute a reasonable epsilon.
        railProjection.simplify(epsilon) # simplify the projected axis.
        retval= railProjection.getBufferPolygon(spread, 0)
        return retval

    def getDeckLoadedContourThroughLayers(self, spreadingLayers, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Return the loaded contour of the rail taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param deckMidplane: mid-plane of the bridge deck.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        ## Append the deck spreading to the list.
        sLayers= spreadingLayers+[(deckThickness/2.0, deckSpreadingRatio)]
        # Call the regular method.
        return self.getLoadedContour(midplane= deckMidplane, spreadingLayers= sLayers)
    
    def getDeckLoadedContourThroughEmbankment(self, embankment, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Return the loaded contour of the rail taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckMidplane: mid-plane of the bridge deck.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        # Compute spreading Layers.
        ## Compute the vertical proyection of the midpoint of the rail on
        ## the deck mid-plane.
        vertLine= geom.Line3d(self.getMidpoint(), geom.Vector3d(0,0,-100.0))
        projPos= deckMidplane.getIntersection(vertLine)

        ## Compute half deck thickness.
        halfDeckThickness= deckThickness/2.0
        ## Ask the embankment about the layer thicknesses in this position.
        layerThicknesses= embankment.getLayerThicknesses(point= projPos+geom.Vector3d(0,0,halfDeckThickness))
        ## Construct the spreading layers list.
        spreadingLayers= list()
        for thk, layer in zip(layerThicknesses, embankment.layers):
            spreadingLayers.append((thk, layer.loadSpreadingRatio))
        # Call the regular method.
        return self.getLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRation)

    def pickLoadedNodes(self, loadedContour, originSet, deckMidplane):
        ''' Define uniform loads on the tracks with the argument values:

        :param loadedContour: 2D polygon defining the loaded region.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        '''
        # Pick loaded nodes.
        retval= list()
        tol= .01
        ref= deckMidplane.getRef()
        for n in originSet.nodes:
            posProj= deckMidplane.getProjection(n.getInitialPos3d)
            nodePos2d= ref.getLocalPosition(posProj)
            if(loadedContour.In(nodePos2d, tol)):  # node in loaded contour.
                retval.append(n)
        return retval

    def computeNodalBrakingLoads(self, loadedNodes, brakingLoad):
        ''' Compute the load vector for each loaded node due to braking.

        :param loadedNodes: nodes that will be loaded.
        :param brakingLoad: total rail load due to braking.
        '''
        numLoadedNodes= len(loadedNodes)
        brakingLoadPerNode= brakingLoad/numLoadedNodes # load for each node.
        retval= list()
        for n in loadedNodes:
            pos= n.getInitialPos3d
            nearestSegment= self.railAxis.getNearestSegment(pos)
            brakingDir= nearestSegment.getIVector
            brakingLoad= brakingLoadPerNode*brakingDir
            loadVector= xc.Vector([brakingLoad.x, brakingLoad.y, brakingLoad.z, 0.0,0.0,0.0])
            retval.append(loadVector)
        return retval
        
    def defDeckRailUniformLoadsThroughLayers(self, spreadingLayers, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        loadedContour= self.getDeckLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes)
        return retval
    
    def defDeckRailUniformLoadsThroughEmbankment(self, embankment, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        loadedContour= self.getDeckLoadedContourThroughEmbankment(embankment= embankment, deckMidplane= deckMidplane, deckThicknesss= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes)
        return retval

    def defDeckRailBrakingLoadsThroughLayers(self, brakingLoad, spreadingLayers, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param brakingLoad: total rail load due to braking.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        loadedContour= self.getDeckLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Compute braking load for each node.
        brakingLoads= self.computeNodalBrakingLoads(loadedNodes= loadedNodes, brakingLoad= brakingLoad)
        # Apply nodal loads.
        retval= list()
        for n, brakingLoad in zip(loadedNodes, brakingLoads):
            retval.append(n.newLoad(brakingLoad))
        return retval
    
    def defBackfillUniformLoads(self, originSet, embankment, delta, eta= 1.0):
        ''' Define backfill loads due the uniform loads on the rail.

        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        '''
        dynamicLoad= self.getDynamicLoad()
        vertexList= self.railAxis.getVertexList()
        if((abs(dynamicLoad)>0) and (len(vertexList)>1)):
            v0= vertexList[0]
            # Compute mid-plane of the loaded wall.
            wallMidplane= originSet.nodes.getRegressionPlane(0.0)
            for v1 in vertexList[1:]:
                segment= geom.Segment3d(v0,v1)
                # Check if segment is too near to the wall.
                d0= wallMidplane.dist(segment.getFromPoint())
                d1= wallMidplane.dist(segment.getToPoint())
                sgLength= segment.getLength()
                tol= 0.5  # too near (arbitrary value -> try to enhance this)
                if((d0<tol) or (d1<tol)):  # Segment too near to the wall.
                    if(d0<tol):
                        newPoint= segment.getPoint(tol)
                        segment= geom.Segment3d(newPoint, v1)
                    else:  # (d1<tol)
                        L= segment.getLength()-tol
                        newPoint= segment.getPoint(L)
                        segment= geom.Segment3d(v0, newPoint)
                loadedLine= boussinesq.LinearLoad(segment= segment, loadValues=[-dynamicLoad, -dynamicLoad], eSize= 0.25)
                # Compute loads on elements.
                loadedLine.appendLoadToCurrentLoadPattern(elements= originSet.elements, eta= eta, delta= delta)
                v0= v1

class UniformRailLoad(RailLoadBase):
    ''' Uniform load along a rail.

    :ivar load: value of the uniform load.
    :ivar directionVector: unitary vector in the direction of the load.
    '''
    def __init__(self, railAxis, load, directionVector= xc.Vector([0, 0, -1]),dynamicFactor= 1.0, classificationFactor= 1.21):
        ''' Constructor.

        :param railAxis: 3D polyline defining the axis of the rail.
        :param load: value of the uniform load.
        :param directionVector: unitary vector in the direction of the load.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        super().__init__(railAxis= railAxis, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.load= load
        self.directionVector= directionVector

    def getNodalLoadVector(self, numNodes):
        ''' Return the load vector at the contact surface.

        :param numNodes: number of loaded nodes.
        '''
        # Compute load vector.
        totalLoad= self.getDynamicLoad()*self.railAxis.getLength()
        nodalLoad= totalLoad/numNodes
        retval= nodalLoad*self.directionVector
        return retval

    def applyNodalLoads(self, loadedNodes):
        ''' Apply the load in the given node list.

        :param loadedNodes: nodes that will be loaded.
        '''
        # Compute load vector
        numLoadedNodes= len(loadedNodes)
        vLoad= self.getNodalLoadVector(numLoadedNodes)
        loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
        # Apply nodal loads.
        retval= list()
        for n in loadedNodes:
            retval.append(n.newLoad(loadVector))
        return retval

class VariableDirectionRailLoad(RailLoadBase):
    ''' Uniform load along a rail with its direction expressed as
        components in the local reference system of each segment.

    :ivar loadComponents: values of the load components in the local
                          reference system of each segment.
    '''
    def __init__(self, railAxis, loadComponents, dynamicFactor= 1.0, classificationFactor= 1.21, orientationVector= None):
        ''' Constructor.

        :param railAxis: 3D polyline defining the axis of the rail.
        :param loadComponents: values of the load components in the local
                               reference system of each segment.
        :param directionVector: unitary vector in the direction of the load.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        :param orientationVector: if not None, the orientation of the unit 
                                  vector J of the coordinate systems is chosen
                                  so that it's dot product with the orientation
                                  vector is positive. See getCoordinateSystems
                                  method.
        '''
        super().__init__(railAxis= railAxis, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.loadComponents= loadComponents
        self.orientationVector= orientationVector

    def getCoordinateSystems(self):
        ''' Return the local coordinate systems along the rail axis.
        '''
        retval= list()
        vertices= self.railAxis.getVertexList()
        currentLength= 0.0
        lastVertex= vertices[0]
        for currentVertex in vertices[1:]:
            segmentLength= currentVertex.dist(lastVertex)
            currentLength+= segmentLength/2.0
            iVector= self.railAxis.getIVectorAtLength(currentLength)
            jVector= self.railAxis.getJVectorAtLength(currentLength)
            if(self.orientationVector): # orient the system if needed.
                if(self.orientationVector.dot(jVector)<0.0):
                    jVector= -jVector
            retval.append(geom.CooSysRect3d3d(iVector, jVector))
            currentLength+= segmentLength/2.0
            lastVertex= currentVertex
        return retval

    def computeLoadVectors(self, loadedNodes):
        ''' Compute the load vectors for each segment in global coordinates.

        '''
        # Compute the load that correspond to each node.
        localLoadVector= geom.Vector3d(self.loadComponents[0], self.loadComponents[1], self.loadComponents[2])
        numLoadedNodes= len(loadedNodes)
        loadFactor= self.railAxis.getLength()/numLoadedNodes
        localLoadVector*= loadFactor

        # Compute load vectors.
        coordinateSystems= self.getCoordinateSystems()
        retval= list()
        for cs in coordinateSystems:
            globalLoadVector= cs.getGlobalCoordinates(localLoadVector)
            retval.append(globalLoadVector)
        return retval

    def applyNodalLoads(self, loadedNodes):
        ''' Apply the load in the given node list.

        :param loadedNodes: nodes that will be loaded.
        '''
        loadVectors= self.computeLoadVectors(loadedNodes= loadedNodes)
        
        # Apply nodal loads.
        retval= list()
        for n in loadedNodes:
            pos= n.getInitialPos3d
            nearestSegmentIndex= self.railAxis.getNearestSegmentIndex(pos)
            vLoad= loadVectors[nearestSegmentIndex]
            loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
            retval.append(n.newLoad(loadVector))
        return retval
    
    def defDeckRailLoadsThroughLayers(self, spreadingLayers, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        loadedContour= self.getDeckLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Apply nodal loads.
        return self.applyNodalLoads(loadedNodes= loadedNodes)
