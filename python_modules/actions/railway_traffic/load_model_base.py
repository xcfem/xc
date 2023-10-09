# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2018,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import sys
import geom
import xc
from actions import loads
from misc_utils import log_messages as lmsg
from geotechnics import horizontal_surcharge as hs
from geotechnics import boussinesq

# Railway traffic loads.
#     ____
#     |DD|____T_
#     |_ |_____|<
#     @-@-@-oo\
#

class WheelLoad(object):
    ''' Load of a wheel.

    :ivar position: position of the wheel
    :ivar load: load
    :ivar nodes: nodes under the wheel.
    '''
    def __init__(self, pos, ld, localCooSystem= None, nodes= None):
        ''' Constructor.

        :param pos: position of the wheel
        :param ld: load
        :param localCooSystem: local coordinate system whose I vector
                               is aligned with the track axis and
                               its XY plane contains the road surface
                               at the wheel position.
        :param nodes: nodes under the wheel.
        '''
        self.position= pos
        self.load= ld
        self.localCooSystem= localCooSystem

    def getLocalReferenceSystem(self):
        ''' Return a local reference system for the wheel with origin in
            the wheel position, x axis along the track axis and y axis normal
            to the x axis on the road surface.'''
        retval= None
        if(self.localCooSystem):
            retval= geom.Ref2d3d(self.position, self.localCooSystem.getIVector(), self.localCooSystem.getJVector())
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; undefined local coordinate system.')
        return retval

    def getLoadedContour(self, spreadingLayers= None):
        ''' Return the loaded contour of the wheel taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

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
        # Compute local axis.
        reference= self.getLocalReferenceSystem()
        trackAxisDir= reference.getLocalCoordinates(self.localCooSystem.getIVector())
        jVector= trackAxisDir  # Longitudinal direction.
        iVector= geom.Vector2d(-jVector.y, jVector.x)  # Transverse direction.
        # Compute spread in each direction.
        transverseSpread= spread
        longitudinalSpread= spread
        # Compute contour points.
        contourPoints= list()
        origin= geom.Pos2d(0,0)
        contourPoints.append(origin+transverseSpread*iVector+longitudinalSpread*jVector)  # +,+
        contourPoints.append(origin-transverseSpread*iVector+longitudinalSpread*jVector)  # -,+
        contourPoints.append(origin-transverseSpread*iVector-longitudinalSpread*jVector)  # -,-
        contourPoints.append(origin+transverseSpread*iVector-longitudinalSpread*jVector)  # +,-
        return reference, geom.Polygon2d(contourPoints)

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        return {'pos':self.position,'load':self.load}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.position= dct['pos']
        self.load= dct['ld']

    def __str__(self):
        return str(self.getDict())

    def pickDeckNodesThroughLayers(self, originSet, spreadingLayers= None):
        ''' Pick the deck nodes loaded by the wheel.

        :param originSet: set to pick the loaded nodes from.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        '''
        retval= list()
        if(self.localCooSystem):
            reference, loadedContour= self.getLoadedContour(spreadingLayers= spreadingLayers)
            tol= .01
            for n in originSet.nodes:
                nodePos2d= reference.getLocalPosition(n.getInitialPos3d)
                if(loadedContour.In(nodePos2d, tol)):  # node in loaded contour.
                    retval.append(n)
        else:
            n= originSet.getNearestNode(self.position)
            retval= [n]
        return retval

    def getDeckLoadedContourThroughLayers(self, spreadingLayers, deckThickness, deckSpreadingRatio= 1/1):
        ''' Return the loaded contour of the wheel taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        ## Append the deck spreading too.
        spreadingLayers.append((deckThickness/2.0, deckSpreadingRatio))
        # Call the regular method.
        return self.getLoadedContour(spreadingLayers= spreadingLayers)

    def getDeckLoadedContourThroughEmbankment(self, embankment, deckMidplane, deckThickness, deckSpreadingRatio= 1/1):
        ''' Return the loaded contour of the wheel taking into account
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
        ## Compute the vertical proyection of the wheel position on the deck
        ## mid-plane.
        vertLine= geom.Line3d(self.position, geom.Vector3d(0, 0, -100.0))
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
        return self.getLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)

    def pickDeckNodesThroughEmbankment(self, originSet, embankment, deckThickness, deckSpreadingRatio= 1/1):
        ''' Pick the nodes loaded by the wheel.

        :param originSet: set to pick the loaded nodes from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckThickness: thickness of the deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        nNodes= len(originSet.nodes)  # Number of nodes in the origin set.
        retval= list()
        if(nNodes>3):
            if(self.localCooSystem):
                # Compute the deck mid-plane as the regression plane
                # of the positions of its nodes.
                deckMidplane= originSet.nodes.getRegressionPlane(0.0)
                # Compute the loaded contour.
                reference, loadedContour= self.getDeckLoadedContourThroughEmbankment(embankment= embankment, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
                tol= .01
                for n in originSet.nodes:
                    nodePos2d= reference.getLocalPosition(n.getInitialPos3d)
                    if(loadedContour.In(nodePos2d, tol)):  # node in loaded contour.
                        retval.append(n)
            else:
                n= originSet.getNearestNode(self.position)
                retval= [n]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; the set: \''+originSet.name+'\' must have at least 3 nodes, it has: '+str(nNodes))
        return retval

    def getLoadVector(self, gravityDir= xc.Vector([0, 0, -1])):
        ''' Return the load vector at the contact surface.


        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # Compute load vector.
        return self.load*gravityDir

    def applyNodalLoads(self, loadedNodes, gravityDir= xc.Vector([0, 0, -1])):
        ''' Create the nodal loads corresponding to the contact pressure.

        :param loadedNodes: nodes that will be loaded.
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        retval= list()
        numLoadedNodes= 0
        if(loadedNodes):
            numLoadedNodes= len(loadedNodes)
        if(numLoadedNodes>0):
            # Compute load vector.
            vLoad= self.getLoadVector(gravityDir= gravityDir)
            loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
            if(numLoadedNodes==1):
                retval.append(loadedNodes[0].newLoad(loadVector))
            else:
                ptCoo= [self.position.x, self.position.y, self.position.z]
                slidingVectorLoad= loads.SlidingVectorLoad(name= 'wheelLoad', nodes= loadedNodes, pntCoord= ptCoo, loadVector= loadVector)
                retval.extend(slidingVectorLoad.appendLoadToCurrentLoadPattern())
        return retval

    def getBackfillConcentratedLoad(self, gravityDir= xc.Vector([0, 0, -1])):
        ''' Return the concentrated loads on the backfill corresponding to
            the wheel loads.

        :param gravityDir: direction of the gravity field (unit vector).
        '''
        if(abs(gravityDir[0])>1e-3) or (abs(gravityDir[1])>1e-3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; gravity directions different from (0,0,-1) not supported yet.')
        vLoad= self.getLoadVector(gravityDir= gravityDir)
        boussinesqLoad= boussinesq.ConcentratedLoad(p= self.position, Q=vLoad[2])
        horizontalLoad= hs.HorizontalConcentratedLoadOnBackfill3D(pos= self.position, H= geom.Vector3d(vLoad[0],vLoad[1],0))
        return (horizontalLoad, boussinesqLoad)
    
    def defDeckConcentratedLoadsThroughLayers(self, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define uniform loads on the tracks with the argument values:

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # Pick loaded nodes.
        loadedNodes= self.pickDeckNodesThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes, gravityDir= gravityDir)
        return retval


class DynamicFactorLoad(object):
    ''' Base class for railway loads.

    :ivar dynamicFactor: dynamic factor affecting the load.
    :ivar classificationFactor: classification factor (on lines carrying rail
                                traffic which is heavier or lighter than
                                normal rail traffic).
    '''
    def __init__(self, dynamicFactor, classificationFactor= 1.21):
        ''' Constructor.

        :param dynamicFactor: dynamic factor.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        self.dynamicFactor= dynamicFactor
        self.classificationFactor= classificationFactor

    def setDynamicFactor(self, dynamicFactor):
        ''' Set the dynamic factor for the load.

        :param dynamicFactor: dynamic factor.
        '''
        self.dynamicFactor= dynamicFactor

    def setClassificationFactor(self, classificationFactor):
        ''' Set the classification factor for the load.

        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        self.classificationFactor= classificationFactor


class UniformRailLoad(DynamicFactorLoad):
    ''' Uniform load along a rail.

    :ivar railAxis: 3D polyline defining the axis of the rail.
    :ivar load: value of the uniform load.
    '''
    def __init__(self, railAxis, load, dynamicFactor= 1.0, classificationFactor= 1.21):
        ''' Constructor.

        :param railAxis: 3D polyline defining the axis of the rail.
        :param load: value of the uniform load.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        super().__init__(dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.railAxis= railAxis
        self.load= load

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
        retval= railProjection.getBufferPolygon(spread, 8)
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
        spreadingLayers.append((deckThickness/2.0, deckSpreadingRatio))
        # Call the regular method.
        return self.getLoadedContour(midplane= deckMidplane, spreadingLayers= spreadingLayers)
    
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

    def getNodalLoadVector(self, numNodes, gravityDir= xc.Vector([0,0,-1])):
        ''' Return the load vector at the contact surface.

        :param numNodes: number of loaded nodes.
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # Compute load vector.
        totalLoad= self.getDynamicLoad()*self.railAxis.getLength()
        nodalLoad= totalLoad/numNodes
        retval= nodalLoad*gravityDir
        return retval

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

    def applyNodalLoads(self, loadedNodes, gravityDir):
        ''' Apply the load in the given node list.

        :param loadedNodes: nodes that will be loaded.
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # Compute load vector
        numLoadedNodes= len(loadedNodes)
        vLoad= self.getNodalLoadVector(numLoadedNodes, gravityDir= gravityDir)
        loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
        # Apply nodal loads.
        retval= list()
        for n in loadedNodes:
            retval.append(n.newLoad(loadVector))
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
        
    def defDeckRailUniformLoadsThroughLayers(self, spreadingLayers, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
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
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        loadedContour= self.getDeckLoadedContourThroughLayers(spreadingLayers= spreadingLayers, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes, gravityDir= gravityDir)
        return retval
    
    def defDeckRailUniformLoadsThroughEmbankment(self, embankment, originSet, deckMidplane, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define uniform loads on the tracks with the argument values:

        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param originSet: set to pick the loaded nodes from.
        :param deckMidplane: midplane of the loaded surface.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        loadedContour= self.getDeckLoadedContourThroughEmbankment(embankment= embankment, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Pick loaded nodes.
        loadedNodes= self.pickLoadedNodes(loadedContour= loadedContour, originSet= originSet, deckMidplane= deckMidplane)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes, gravityDir= gravityDir)
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
    
    def defBackfillUniformLoads(self, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0,0,-1])):
        ''' Define backfill loads due the uniform loads on the rail.

        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field (unit dir).
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

class TrainLoadModel(object):
    ''' Model for the loads of a train.

    :ivar locomotive: locomotive model.
    :ivar uniformLoad: uniform load on the track.
    '''
    def __init__(self, locomotive, uniformLoad, dynamicFactor, classificationFactor= 1.21):
        ''' Constructor:

        :param locomotive: locomotive model.
        :param uniformLoad: uniform load on the track.

        :param dynamicFactor: dynamic factor.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        self.locomotive= locomotive
        # The dynamic factor is stored in the locomotive.
        self.locomotive.setDynamicFactor(dynamicFactor)
        self.locomotive.setClassificationFactor(classificationFactor)
        self.uniformLoad= uniformLoad

    def getDynamicFactor(self):
        ''' Return the dynamic factor.'''
        return self.locomotive.dynamicFactor

    def getClassificationFactor(self):
        ''' Return the classification factor.'''
        return self.locomotive.classificationFactor

    def getRailUniformLoad(self):
        ''' Return the raw uniform load.'''
        return self.uniformLoad/2.0

    def getDynamicUniformLoad(self):
        ''' Return the uniform load affected by the dynamic factor.'''
        return self.getClassificationFactor()*self.getDynamicFactor()*self.uniformLoad
    
    def getWheelLoads(self, ref, loadFactor= 1.0):
        ''' Return the loads of the wheels of the tandem along with its
            positions.

        :param ref: reference system at the center of the locomotive.
        :param loadFactor: factor to apply to the loads.
        '''
        return self.locomotive.getWheelLoads(ref= ref, loadFactor= loadFactor)

    def getTotalLoad(self, trackLength):
        ''' Return the total load of the train over the given length.

        :param trackLength: length of the track that supports the train.
        '''
        locomotiveLoad= self.locomotive.getTotalLoad()
        uniformLoadedLength= trackLength-self.locomotive.getTotalLength()
        uniformLoad= self.getDynamicUniformLoad()*uniformLoadedLength
        return locomotiveLoad+uniformLoad
    
# Rudimentary implementation of the track axis concept.

class TrackAxis(object):
    ''' Track axis.

    :ivar segment: 3D segment defining the axis.
    '''
    def __init__(self, trackAxis):
        ''' Constructor.

        :param trackAxis: 3D polyline defining the axis of the track.
        '''
        self.trackAxis= trackAxis
        tol= self.trackAxis.getLength()/1e4
        self.trackAxis.removeRepeatedVertexes(tol)

    def getLength(self):
        ''' Return the length of the axis track segment.'''
        return self.trackAxis.getLength()

    def getTotalLoad(self, trainModel):
        ''' Return the total load of the given train over this track.

        :param trainModel: trainModel on this track.
        '''
        return trainModel.getTotalLoad(trackLength= self.getLength())

    def getReferenceAt(self, lmbdArcLength):
        ''' Return a 3D reference system with origin at the point
            O+lmbdArcLength*L where O is the first point of the reference
            axis and lmbdArcLength is a value between 0 and 1. If
            lmbdArcLength=0 the returned point is the origin of the track
            axis, and if lmbdArcLength= 1 it returns its end. The axis
            of the reference system are (or will be) defined as follows:

            - x: tangent to the axis oriented towards its end.
            - y: normal to the axis oriented towards its center.
            - z: defined by the cross-product x^y.

        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        '''
        lng= lmbdArcLength*self.trackAxis.getLength()
        org= self.trackAxis.getPointAtLength(lng)
        iVector= self.trackAxis.getIVectorAtLength(lng)
        jVector= self.trackAxis.getJVectorAtLength(lng)
        return geom.Ref2d3d(org, iVector, jVector)

    def getVDir(self, relativePosition= 0.5):
        ''' Return the direction vector of the track axis.

        :param relativePosition: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        '''
        return self.trackAxis.getIVectorAtLength(relativePosition*self.trackAxis.getLength())

    def getWheelLoads(self, trainModel:TrainLoadModel, relativePosition, loadFactor= 1.0):
        ''' Return the wheel loads of load model argument in the position
            specified by the relativePosition parameter.

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        :param loadFactor: factor to apply to the loads.
        '''
        wheelLoads= list()
        posSides= list()
        sideSum= 0
        ref= self.getReferenceAt(lmbdArcLength= relativePosition)
        return trainModel.getWheelLoads(ref= ref, loadFactor= loadFactor)

    def getRailAxes(self, trackGauge= 1.435):
        ''' Return a 3D polyline representing the rail axis.

        :param trackGauge: track gauge (defaults to the international standard gauge 1435 mm).
        '''
        offsetDist= trackGauge/2.0
        vertexList= self.trackAxis.getVertexList()
        sz= len(vertexList)
        if(sz>2):
            planePolyline= geom.PlanePolyline3d(vertexList)
        elif(sz>1):
            ref= self.getReferenceAt(lmbdArcLength= 0.5)
            p0= ref.getLocalPosition(vertexList[0])
            p1= ref.getLocalPosition(vertexList[1])
            planePolyline= geom.PlanePolyline3d(ref, geom.Polyline2d([p0, p1]))
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; the track axis must have 2 vertices at least.')
        rail1= planePolyline.offset(-offsetDist)
        rail2= planePolyline.offset(offsetDist)
        return rail1, rail2

    def getRailChunks(self, trainModel, relativePosition):
        ''' Return the rail segments that are not occupied by the locomotive.

        :param trainModel: trainModel on this track.
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        '''
        # Compute the axes of the rails.
        rail1, rail2= self.getRailAxes()
        railChunks= list()  # Uniform loaded rail chunks.
        if(relativePosition is None):  # No locomotive in this segment.
            railChunks.append(rail1)
            railChunks.append(rail2)
        else:
            # Compute planes at locomotive front and back.
            halfLocomotiveLength= trainModel.locomotive.getTotalLength()/2.0
            ref= self.getReferenceAt(lmbdArcLength= relativePosition)
            org= ref.Org
            jVector= ref.getJVector()
            kVector= ref.getKVector()
            pointAtFront= ref.getGlobalPosition(geom.Pos2d(halfLocomotiveLength,0))
            planeAtFront= geom.Plane3d(pointAtFront, jVector, kVector)
            pointAtBack= ref.getGlobalPosition(geom.Pos2d(-halfLocomotiveLength,0))
            planeAtBack= geom.Plane3d(pointAtBack, jVector, kVector)
            # Get the rails that are outside the locomotive.
            for rail in [rail1, rail2]:
                railFromPoint= rail.getFromPoint()
                for plane in [planeAtFront, planeAtBack]:
                    orgSide= plane.getSide(org)  # This one is in the locomotive center.
                    targetSide= -orgSide  # So we search for this side.
                    intList= rail.getIntersection(plane)
                    if(len(intList)>0):  # intersection found.
                        intPoint= intList[0]
                        fromPointSide= plane.getSide(railFromPoint)
                        if(fromPointSide==targetSide):
                            targetChunk= rail.getLeftChunk(intPoint,.01)
                        else:
                            targetChunk= rail.getRightChunk(intPoint, .01)
                        railChunks.append(targetChunk)
                    else:  # Locomotive is longer than the rail => no intersection.
                        intPoint= None
        return railChunks

    def getRailUniformLoads(self, trainModel, relativePosition, gravityDir= xc.Vector([0,0,-1])):
        ''' Return the uniform loads on the track rails.

        :param trainModel: trainModel on this track.
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # Get the rails that are outside the locomotive.
        railChunks= self.getRailChunks(trainModel, relativePosition)  # Uniform loaded rail chunks.
        # Create the uniform rail loads.
        qRail= trainModel.getRailUniformLoad()
        retval= list()
        for rc in railChunks:
            unifRailLoad= UniformRailLoad(railAxis= rc.getPolyline3d(), load= qRail, dynamicFactor= trainModel.getDynamicFactor(), classificationFactor= trainModel.getClassificationFactor())
            retval.append(unifRailLoad)
        return retval
    
    def getRailsBrakingLoads(self):
        ''' Return the uniform loads object for both track rails.
        '''
        # Get the rails that are outside the locomotive.
        railAxes= self.getRailChunks(trainModel= None, relativePosition= None)  # Get raw rail axes.
        # Create the uniform rail loads.
        retval= list()
        for rc in railAxes:
            brakingRailLoad= UniformRailLoad(railAxis= rc.getPolyline3d(), load= 0.0, dynamicFactor= 1.0, classificationFactor= 1.0)
            retval.append(brakingRailLoad)
        return retval

    def defDeckWheelLoadsThroughLayers(self, trainModel, relativePosition, spreadingLayers, originSet, deckThickness, deckSpreadingRatio, gravityDir= xc.Vector([0,0,-1])):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        :param trainModel: trainModel on this track.
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        wheelLoads= self.getWheelLoads(trainModel= trainModel, relativePosition= relativePosition)
        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir))
        return retval
    
    def defDeckRailUniformLoadsThroughLayers(self, trainModel:TrainLoadModel, relativePosition, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define uniform loads on the tracks with the argument values:

        :param trainModel: trainModel on this track (see TrainLoadModel class).
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, gravityDir= gravityDir)
        retval= list()
        for rul in railUniformLoads:
            retval.extend(rul.defDeckRailUniformLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir))
        return retval
    
    def defDeckRailsBrakingLoadThroughLayers(self, brakingLoad:float, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param brakingLoad: total braking load.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        railBrakingLoads= self.getRailsBrakingLoads()
        numRails= len(railBrakingLoads)
        brakingLoadPerRail= brakingLoad/numRails
        retval= list()
        for rbl in railBrakingLoads:
            retval.extend(rbl.defDeckRailBrakingLoadsThroughLayers(brakingLoad= brakingLoadPerRail, spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
            
    def defDeckRailUniformLoadsThroughEmbankment(self, trainModel, relativePosition, embankment, originSet, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define uniform loads on the tracks with the argument values:

        :param trainModel: trainModel on this track.
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, gravityDir= gravityDir)
        retval= list()
        for rul in railUniformLoads:
            retval.extend(rul.defDeckRailUniformLoadsThroughEmbankment(embankment= embankment, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir))
        return retval
            
    def defDeckLoadsThroughLayers(self, trainModel, relativePosition, spreadingLayers, deckThickness, deckSpreadingRatio= 1/1, originSet= None, gravityDir= xc.Vector([0,0,-1])):
        ''' Define punctual and uniform loads.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePosition: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # concentrated loads.
        retval= self.defDeckWheelLoadsThroughLayers(trainModel= trainModel, relativePosition= relativePosition, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir)
        # uniform load.
        retval.extend(self.defDeckRailUniformLoadsThroughLayers(trainModel= trainModel, relativePosition= relativePosition, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir))
        return retval

    def defDeckBrakingLoadThroughLayers(self, brakingLoad, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define uniform loads on the tracks with the argument values:

        :param brakingLoad: total rail load due to braking.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        return self.defDeckRailsBrakingLoadThroughLayers(brakingLoad= brakingLoad, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)


    def defBackfillUniformLoads(self, trainModel, relativePosition, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0,0,-1])):
        ''' Define backfill loads due the uniform load on the track.

        :param trainModel: trainModel on this track.
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, gravityDir= gravityDir)
        setMidplane= originSet.nodes.getRegressionPlane(0.0)
        for rul in railUniformLoads:
            rul.clip(setMidplane)  # Avoid "negative" pressures over the wall.
            rul.defBackfillUniformLoads(originSet= originSet, embankment= embankment, delta= delta, eta= eta, gravityDir= gravityDir)


class TrackAxes(object):
    ''' Track axis container base class (abstract class).

    :ivar trackAxes: list of track axes.
    '''
    def __init__(self, trackAxesPolylines):
        ''' Constructor.

        :param railAxesPolylines: 3D polylines defining the axis of track.
        '''
        self.trackAxes= list()
        for trackAxisPline in trackAxesPolylines:
            self.trackAxes.append(TrackAxis(trackAxisPline))

    def getLengths(self):
        ''' Return the lengths of the track axes.'''
        retval= list()
        for axis in self.trackAxes:
            retval.append(axis.getArea())
        return retval

    def getNumberTracks(self):
        ''' Return the number of tracks.'''
        return len(self.trackAxes)

    def getWheelLoads(self, trainModels, relativePositions, wallMidplane):
        ''' Return a the wheel loads due to the locomotives of the trains
            in the argument placed at the positions argument.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param wallMidplane: mid-plane of the loaded wall (to select the wheels
                             at one side of the wall).
        '''
        retval= list()
        locomotives= list()
        for tm in trainModels:
            locomotives.append(tm.locomotive)
        # Compute load positions in each track.
        for ta,rpos,td in zip(self.trackAxes, relativePositions, locomotives):
            locomotiveLoads= list()
            if(td is not None):
                locomotiveLoads= ta.getWheelLoads(loadModel= td, relativePosition= rpos, wallMidplane= wallMidplane)
            retval.extend(locomotiveLoads)
        return retval

    def defDeckWheelLoadsThroughLayers(self, trainModels, relativePositions, spreadingLayers= None, originSet= None, deckThickness= 0.0, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        wheelLoads= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= None)
        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir))
        return retval

    def getBackfillConcentratedLoads(self, trainModels, relativePositions, wallMidplane, gravityDir= xc.Vector([0,0,-1])):
        ''' Return the wheel loads on the backfill due to the train models
        argument placed at the positions argument.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param wallMidplane: mid-plane of the loaded wall (to select the wheels
                             at one side of the wall).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        wheelLoads= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= wallMidplane)
        retval= list()
        for wheelLoad in wheelLoads:
            retval.append(wheelLoad.getBackfillConcentratedLoad(gravityDir= gravityDir))
        return retval

    def defDeckRailUniformLoadsThroughEmbankment(self, trainModels, relativePositions, embankment, originSet, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1])):
        ''' Define uniform loads on the tracks with the argument values:

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param originSet: set to pick the loaded nodes from.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        for ta, tm, rp in zip(self.trackAxes, trainModels, relativePositions):
            q= tm.getDynamicUniformLoad()
            if(abs(q)>0.0):
                ta.defDeckRailUniformLoadsThroughEmbankment(trainModel= tm, relativePosition= rp, embankment= embankment, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir)

    def defDeckConcentratedLoadsThroughLayers(self, trainModels, relativePositions, originSet= None, deckThickness= 0.0, deckSpreadingRation= 1/1, gravityDir= xc.Vector([0,0,-1]), spreadingLayers= None):
        ''' Define punctual loads under the wheels.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set with the nodes to pick from.
        :param deckThickness: thickness of the bridge deck.
        :param gravityDir: direction of the gravity field (unit vector).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        '''
        return self.defDeckWheelLoadsThroughLayers(trainModels= trainModels, relativePositions= relativePositions, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= 1/1, gravityDir= gravityDir)

    def defDeckLoadsThroughEmbankment(self, trainModels, relativePositions, trackUniformLoads, embankment, deckThickness, deckSpreadingRatio= 1/1, originSet= None, gravityDir= xc.Vector([0,0,-1]), spreadingLayers= None):
        ''' Define punctual and uniform loads.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of each locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param trackUniformLoads: load for each track [1st, 2nd, 3rd,...].
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field (unit vector).
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                bridge deck.
        '''
        # concentrated loads.
        self.defDeckConcentratedLoadsThroughLayers(trainModels= trainModels, relativePositions= relativePositions, originSet= originSet, gravityDir= gravityDir, spreadingLayers= spreadingLayers)
        # uniform load.
        self.defDeckRailUniformLoadsThroughEmbankment(trainModels= trainModels, relativePositions= relativePositions, embankment= embankment, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir)

    def getDeckWheelLoadsThroughEmbankment(self, trainModels, relativePositions, embankment, deckThickness, deckSpreadingRatio= 1/1, originSet= None):
        ''' Return a dictionary containing the wheel loads due to the locomotives
            argument in the positions argument.

        :param trainModels: trainModels on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        '''
        retval= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= None)
        if(originSet):  # pick the loaded by each wheel
            for load in retval:
                load.pickDeckNodesThroughEmbankment(originSet= originSet, embankment= embankment, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        return retval

    def defDeckConcentratedLoadsThroughEmbankment(self, trainModels, relativePositions, embankment, deckThickness, deckSpreadingRatio= 1/1, originSet= None, gravityDir= xc.Vector([0,0,-1])):
        ''' Define punctual loads under the wheels.

        :param trainModels: train models on each track (train model 1-> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckThickness: thickness of the deck bridge.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        wheelLoads= self.getDeckWheelLoadsThroughEmbankment(trainModels= trainModels, relativePositions= relativePositions, embankment= embankment, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, originSet= originSet)
        retval= list()
        for wl in wheelLoads:
            retval.append(wl.defNodalLoads(gravityDir= gravityDir))
        return retval

    def defDeckLoadsThroughEmbankment(self, trainModels, relativePositions, embankment, originSet, deckThickness, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0, 0, -1])):
        ''' Define punctual and uniform loads.
        :param trainModels: trainModels on each track (trainModel1 -> track 1,
                            trainModel 2 -> track 2 and so on).
        :param relativePositions: relative positions of each locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param trackUniformLoads: load for each track [1st, 2nd, 3rd,...].
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param originSet: set containing the nodes to pick from.
        :param deckThickness: thickness of the deck bridge.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # punctual loads.
        self.defDeckConcentratedLoadsThroughEmbankment(trainModels= trainModels, relativePositions= relativePositions, embankment= embankment, deckThickness= deckThickness, deckSpreadingRatio=deckSpreadingRatio, originSet= originSet, gravityDir= gravityDir)
        # uniform load.
        self.defDeckRailUniformLoadsThroughEmbankment(trainModels= trainModels, relativePositions= relativePositions, embankment= embankment, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, gravityDir= gravityDir)

    def defBackfillConcentratedLoads(self, trainModels, relativePositions, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0, 0, -1])):
        ''' Define punctual loads under the wheels.
        :param trainModels: train models on each track (trainModel1 -> track 1,
                            trainModel2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        setMidplane= originSet.nodes.getRegressionPlane(0.0)
        backfillLoads= self.getBackfillConcentratedLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= setMidplane, gravityDir= gravityDir)
        phi= embankment.layers[0].soil.phi
        sz= len(backfillLoads)
        avgLoadedAreaRatio= 0.0
        for bfl in backfillLoads:
            horizontalLoad= bfl[0]
            avgLoadedAreaRatio+= horizontalLoad.appendLoadToCurrentLoadPattern(elements= originSet.elements, phi= phi, delta= delta)
            verticalLoad= bfl[1]
            verticalLoad.appendLoadToCurrentLoadPattern(elements= originSet.elements, eta= eta, delta= delta)
        if(sz):
            avgLoadedAreaRatio/= sz
        return avgLoadedAreaRatio

    def defBackfillUniformLoads(self, trainModels, relativePositions, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0, 0, -1])):
        ''' Define backfill loads due the uniform loads on the tracks.

        :param trainModels: train models on each track (trainModel1 -> track 1,
                            trainModel2 -> track 2 and so on).
        :param relativePositions: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        for trackAxis, tm, rp in zip(self.trackAxes, trainModels, relativePositions):
            trackAxis.defBackfillUniformLoads(trainModel= tm, relativePosition= rp, originSet= originSet, embankment= embankment, delta= delta, eta= eta, gravityDir= gravityDir)

    def defBackfillLoads(self, trainModels, relativePositions, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0, 0, -1])):
        ''' Define punctual and uniform loads.

        :param trainModels: train model on each track (train model 1 -> track 1,
                            train model 2 -> track 2 and so on).
        :param relativePositions: relative positions of each locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        # punctual loads.
        self.defBackfillConcentratedLoads(trainModels= trainModels, relativePositions= relativePositions, originSet= originSet, embankment= embankment, delta= delta, eta= eta, gravityDir= gravityDir)
        # uniform load.
        self.defBackfillUniformLoads(trainModels= trainModels, relativePositions= relativePositions, originSet= originSet, embankment= embankment, delta= delta, eta= eta, gravityDir= gravityDir)
