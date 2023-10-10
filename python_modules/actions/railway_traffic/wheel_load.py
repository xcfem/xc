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

class WheelLoadBase(object):
    ''' Base class for loaded railway wheels.

    :ivar position: position of the wheel
    :ivar nodes: nodes under the wheel.
    '''
    def __init__(self, pos, localCooSystem= None, nodes= None):
        ''' Constructor.

        :param pos: position of the wheel
        :param localCooSystem: local coordinate system whose I vector
                               is aligned with the track axis and
                               its XY plane contains the road surface
                               at the wheel position.
        :param nodes: nodes under the wheel.
        '''
        self.position= pos
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
        return {'pos':self.position}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.position= dct['pos']

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


class WheelLoad(WheelLoadBase):
    ''' Loaded railway wheel.

    :ivar load: absolute value of the load.
    :ivar directionVector: unitary vector in the direction of the load.
    '''
    def __init__(self, pos, ld, directionVector= xc.Vector([0, 0, -1]), localCooSystem= None, nodes= None):
        ''' Constructor.

        :param pos: position of the wheel
        :param ld: load
        :param directionVector: unitary vector in the direction of the load.
        :param localCooSystem: local coordinate system whose I vector
                               is aligned with the track axis and
                               its XY plane contains the road surface
                               at the wheel position.
        :param nodes: nodes under the wheel.
        '''
        super().__init__(pos= pos, localCooSystem= localCooSystem, nodes= nodes)
        self.load= ld
        self.directionVector= directionVector

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= super().getDict()
        retval.update({'load':self.load, 'directionVector':[directionVector[0], directionVector[1], directionVector[2]]})
        return retval

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        super().setFromDict(dct)
        self.load= dct['ld']
        self.directionVector= xc.Vector(dct['directionVector'])

    def getLoadVector(self):
        ''' Return the load vector at the contact surface. '''
        # Compute load vector.
        return self.load*self.directionVector

    def applyNodalLoads(self, loadedNodes):
        ''' Create the nodal loads corresponding to the contact pressure.

        :param loadedNodes: nodes that will be loaded.
        '''
        retval= list()
        numLoadedNodes= 0
        if(loadedNodes):
            numLoadedNodes= len(loadedNodes)
        if(numLoadedNodes>0):
            # Compute load vector.
            vLoad= self.getLoadVector()
            loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
            if(numLoadedNodes==1):
                retval.append(loadedNodes[0].newLoad(loadVector))
            else:
                ptCoo= [self.position.x, self.position.y, self.position.z]
                slidingVectorLoad= loads.SlidingVectorLoad(name= 'wheelLoad', nodes= loadedNodes, pntCoord= ptCoo, loadVector= loadVector)
                retval.extend(slidingVectorLoad.appendLoadToCurrentLoadPattern())
        return retval

    def getBackfillConcentratedLoad(self):
        ''' Return the concentrated loads on the backfill corresponding to
            the wheel loads.
        '''
        if(abs(self.directionVector[0])>1e-3) or (abs(self.directionVector[1])>1e-3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; gravity directions different from (0,0,-1) not supported yet.')
        vLoad= self.getLoadVector()
        boussinesqLoad= boussinesq.ConcentratedLoad(p= self.position, Q=vLoad[2])
        horizontalLoad= hs.HorizontalConcentratedLoadOnBackfill3D(pos= self.position, H= geom.Vector3d(vLoad[0],vLoad[1],0))
        return (horizontalLoad, boussinesqLoad)
    
    def defDeckConcentratedLoadsThroughLayers(self, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
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
        '''
        # Pick loaded nodes.
        loadedNodes= self.pickDeckNodesThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet)
        # Apply nodal loads.
        retval= self.applyNodalLoads(loadedNodes= loadedNodes)
        return retval

