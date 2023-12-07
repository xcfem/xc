# -*- coding: utf-8 -*-
''' Rudimentary implementation of the track axis concept.'''

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
from actions.railway_traffic import train_load_model as tm
from actions.railway_traffic import uniform_rail_load as url
from actions.railway_traffic import track_cross_section_geometry as tcs

class TrackAxis(object):
    ''' Track axis.

    :ivar segment: 3D polyline defining the axis of the track.
    '''
    def __init__(self, trackAxis, trackGauge= 1.435, u= 0.0):
        ''' Constructor.

        :param trackAxis: 3D polyline defining the axis of the track.
        :param trackGauge: track gauge (defaults to the international standard gauge 1435 mm).
        :param u: # cant. Defaults to zero.
        '''
        self.trackAxis= trackAxis
        tol= self.trackAxis.getLength()/1e4
        self.trackAxis.removeRepeatedVertexes(tol)
        self.trackGauge= trackGauge
        self.u= u

    def getLength(self):
        ''' Return the length of the axis track segment.'''
        return self.trackAxis.getLength()

    def getTotalLoad(self, trainModel:tm.TrainLoadModel):
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

    def getTrackCrossSection(self):
        ''' Return the cross-section of the track.'''
        return tcs.TrackCrossSection(s= self.trackGauge+.04, u= self.u)

    def getWheelLoads(self, trainModel:tm.TrainLoadModel, relativePosition, loadFactor= 1.0, directionVector= xc.Vector([0,0,-1])):
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
        return trainModel.getWheelLoads(ref= ref, loadFactor= loadFactor, directionVector= directionVector)

    def getRailAxes(self):
        ''' Return a 3D polyline representing the rail axis.

        '''
        offsetDist= self.trackGauge/2.0
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

    def getRailChunks(self, trainModel:tm.TrainLoadModel, relativePosition):
        ''' Return the rail segments that are not occupied by the locomotive.

        :param trainModel: load model of the train (see TrainLoadModel class).
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

    def getRailUniformLoads(self, trainModel: tm.TrainLoadModel, relativePosition, directionVector= xc.Vector([0,0,-1])):
        ''' Return the uniform loads on the track rails.

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param directionVector: unitary vector in the direction of the load.
        '''
        # Get the rails that are outside the locomotive.
        railChunks= self.getRailChunks(trainModel, relativePosition)  # Uniform loaded rail chunks.
        # Create the uniform rail loads.
        qRail= trainModel.getRailUniformLoad()
        retval= list()
        for rc in railChunks:
            unifRailLoad= url.UniformRailLoad(railAxis= rc.getPolyline3d(), load= qRail, directionVector= directionVector, dynamicFactor= trainModel.getDynamicFactor(), classificationFactor= trainModel.getClassificationFactor())
            retval.append(unifRailLoad)
        return retval

    def getRailCentrifugalLoads(self, leftRailCentrifugalLoad, rightRailCentrifugalLoad, trainModel, relativePosition, overrideDynamicFactor= None):
        ''' Return the uniform loads on the track rails due to the given
            centrifugal loads.

        :param leftCentrifugalLoad: centrifugal load on the left rail.
        :param rightCentrifugalLoad: centrifugal load on the right rail.
        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: relative position of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        '''
        # Get the rails that are outside the locomotive.
        railChunks= self.getRailChunks(trainModel, relativePosition)  # Uniform loaded rail chunks.
        # Create the centrifugal rail loads.
        retval= list()
        dynamicFactor= trainModel.getDynamicFactor()
        if(overrideDynamicFactor):
            dynamicFactor= overrideDynamicFactor
        numLeftRailChunks= len(railChunks)/2
        for i, rc in enumerate(railChunks):
            if(i<numLeftRailChunks):
                loadComponents= [0.0, -rightRailCentrifugalLoad.x, -rightRailCentrifugalLoad.y]
            else:
                loadComponents= [0.0, -leftRailCentrifugalLoad.x, -leftRailCentrifugalLoad.y]
            centrifugalRailLoad= url.VariableDirectionRailLoad(railAxis= rc.getPolyline3d(), loadComponents= loadComponents, dynamicFactor= dynamicFactor, classificationFactor= trainModel.getClassificationFactor())
            retval.append(centrifugalRailLoad)
        return retval
    
    def getRailsBrakingLoads(self):
        ''' Return the uniform loads object for both track rails.
        '''
        # Get the rails disregarding the position of the locomotive.
        railAxes= self.getRailChunks(trainModel= None, relativePosition= None)  # Get raw rail axes.
        # Create the uniform rail loads.
        retval= list()
        for rc in railAxes:
            brakingRailLoad= url.UniformRailLoad(railAxis= rc.getPolyline3d(), load= 0.0, dynamicFactor= 1.0, classificationFactor= 1.0)
            retval.append(brakingRailLoad)
        return retval

    def defDeckWheelLoadsThroughLayers(self, trainModel:tm.TrainLoadModel, relativePosition, spreadingLayers, originSet, deckThickness, deckSpreadingRatio, directionVector= xc.Vector([0,0,-1])):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        :param trainModel: load model of the train (see TrainLoadModel class).
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
        :param directionVector: unitary vector in the direction of the load.
        '''
        # Compute the loads on the wheels.
        wheelLoads= self.getWheelLoads(trainModel= trainModel, relativePosition= relativePosition, directionVector= directionVector)
        # Distribute the loads over the nodes in originSet.
        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
    
    def defDeckRailUniformLoadsThroughLayers(self, trainModel:tm.TrainLoadModel, relativePosition, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1, directionVector= xc.Vector([0,0,-1])):
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
        :param directionVector: unitary vector in the direction of the load.
        '''
        # Compute rail uniform loads.
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, directionVector= directionVector)
        # Distribute the load over deck nodes.
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        retval= list()
        for rul in railUniformLoads:
            retval.extend(rul.defDeckRailUniformLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
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
        # Get braking loads on each rail.
        railBrakingLoads= self.getRailsBrakingLoads()
        numRails= len(railBrakingLoads)
        brakingLoadPerRail= brakingLoad/numRails
        # Apply loads to the originSet nodes.
        retval= list()
        for rbl in railBrakingLoads:
            retval.extend(rbl.defDeckRailBrakingLoadsThroughLayers(brakingLoad= brakingLoadPerRail, spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
            
    def defDeckRailUniformLoadsThroughEmbankment(self, trainModel, relativePosition, embankment, originSet, deckThickness, deckSpreadingRatio= 1/1, directionVector= xc.Vector([0,0,-1])):
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
        :param directionVector: unitary vector in the direction of the load.
        '''
        # Compute rail uniform loads.
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, directionVector= directionVector)
        # Distribute the load over deck nodes.
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        retval= list()
        for rul in railUniformLoads:
            retval.extend(rul.defDeckRailUniformLoadsThroughEmbankment(embankment= embankment, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
            
    def defDeckLoadsThroughLayers(self, trainModel, relativePosition, spreadingLayers, deckThickness, deckSpreadingRatio= 1/1, originSet= None, directionVector= xc.Vector([0,0,-1])):
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
        :param directionVector: unitary vector in the direction of the load.
        '''
        # concentrated loads.
        retval= self.defDeckWheelLoadsThroughLayers(trainModel= trainModel, relativePosition= relativePosition, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, directionVector= directionVector)
        # uniform load.
        retval.extend(self.defDeckRailUniformLoadsThroughLayers(trainModel= trainModel, relativePosition= relativePosition, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, directionVector= directionVector))
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

    def defBackfillUniformLoasds(self, trainModel, relativePosition, originSet, embankment, delta, eta= 1.0, directionVector= xc.Vector([0,0,-1])):
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
        :param directionVector: unitary vector in the direction of the load.
        '''
        railUniformLoads= self.getRailUniformLoads(trainModel= trainModel, relativePosition= relativePosition, directionVector= directionVector)
        setMidplane= originSet.nodes.getRegressionPlane(0.0)
        for rul in railUniformLoads:
            rul.clip(setMidplane)  # Avoid "negative" pressures over the wall.
            rul.defBackfillUniformLoads(originSet= originSet, embankment= embankment, delta= delta, eta= eta)
            
    def getRailWindLoads(self, leftRailWindLoad, rightRailWindLoad, trainModel, windDirection= None):
        ''' Return the uniform loads on the track rails due to the given
            wind loads.

        :param leftWindLoad: wind load on the left rail.
        :param rightWindLoad: wind load on the right rail.
        :param trainModel: load model of the train (see TrainLoadModel class).
        :param windDirection: wind direction vector (not necessary if the 
                              curvature of the track does not change sign).
        '''
        # Get the rails disregarding the position of the locomotive.
        railAxes= self.getRailChunks(trainModel= None, relativePosition= None)          # Create the wind rail loads.
        retval= list()
        numLeftRailAxes= len(railAxes)/2
        for i, rc in enumerate(railAxes):
            if(i<numLeftRailAxes):
                loadComponents= [0.0, -rightRailWindLoad.x, -rightRailWindLoad.y]
            else:
                loadComponents= [0.0, -leftRailWindLoad.x, -leftRailWindLoad.y]
            windRailLoad= url.VariableDirectionRailLoad(railAxis= rc.getPolyline3d(), loadComponents= loadComponents, dynamicFactor= 1.0, classificationFactor= 1.0, orientationVector= windDirection)
            retval.append(windRailLoad)
        return retval
    
    def defDeckWindLoadOnRailsThroughLayers(self, trainModel, windPressure:float, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1, windDirection= None):
        ''' Define the nodal loads that correspond to uniform centrifugal
           loads on the rails.

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param windPressure: pressure of the wind on the rolling stock (N/m2).
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
        :param windDirection: wind direction vector (not necessary if the 
                              curvature of the track does not change sign).
        '''
        # Uniform wind load on the rails.
        trackCrossSection= self.getTrackCrossSection()
        railWindLoadsPerMeter= trainModel.getWindLoadPerMeter(windPressure= windPressure, trackCrossSection= trackCrossSection)
        # Create the wind rail loads.
        railWindLoads= self.getRailWindLoads(leftRailWindLoad= railWindLoadsPerMeter[0], rightRailWindLoad= railWindLoadsPerMeter[1], trainModel= trainModel, windDirection= windDirection)
        retval= list()
        # Apply loads to the originSet nodes.
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        for rcl in railWindLoads:
            retval.extend(rcl.defDeckRailLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
    
    def defDeckWindLoadThroughLayers(self, trainModel, windPressure:float, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1, windDirection= None):
        ''' Define wind loads on the bridge deck given:

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param windPressure: pressure of the wind on the rolling stock (N/m2).
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
        :param windDirection: wind direction vector (not necessary if the 
                              curvature of the track does not change sign).
        '''
        # Uniform wind load on the rails.
        return self.defDeckWindLoadOnRailsThroughLayers(trainModel= trainModel, windPressure= windPressure, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio, windDirection= windDirection)
