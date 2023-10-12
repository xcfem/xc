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

class TrackAxes(object):
    ''' Track axis container.

    :ivar trackAxes: list of track axes.
    '''
    def __init__(self, trackAxesPolylines):
        ''' Constructor.

        :param railAxesPolylines: 3D polylines defining the axis of track.
        '''
        self.trackAxes= list()
        for trackAxisPline in trackAxesPolylines:
            self.trackAxes.append(ta.TrackAxis(trackAxisPline))

    def getLengths(self):
        ''' Return the lengths of the track axes.'''
        retval= list()
        for axis in self.trackAxes:
            retval.append(axis.getArea())
        return retval

    def getNumberTracks(self):
        ''' Return the number of tracks.'''
        return len(self.trackAxes)

    def getWheelLoads(self, trainModels, relativePositions, wallMidplane, gravityDir= xc.Vector([0,0,-1])):
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
                locomotiveLoads= ta.getWheelLoads(loadModel= td, relativePosition= rpos, wallMidplane= wallMidplane, gravityDir= gravityDir)
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
        wheelLoads= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= None, gravityDir= gravityDir)
        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
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
        wheelLoads= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= wallMidplane, gravityDir= gravityDir)
        retval= list()
        for wheelLoad in wheelLoads:
            retval.append(wheelLoad.getBackfillConcentratedLoad())
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

    def getDeckWheelLoadsThroughEmbankment(self, trainModels, relativePositions, embankment, deckThickness, deckSpreadingRatio= 1/1, originSet= None, gravityDir= xc.Vector([0,0,-1])):
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
        :param gravityDir: direction of the gravity field (unit vector).
        '''
        retval= self.getWheelLoads(trainModels= trainModels, relativePositions= relativePositions, wallMidplane= None, gravityDir= gravityDir)
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
