# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to Eurocode 1 part 2.'''

__author__= "Luis C. Pérez Tato (LP) "
__copyright__= "Copyright 2015, LP"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from scipy import constants
from actions.railway_traffic import locomotive_load as ll
from actions.railway_traffic import train_load_model as tlm
from actions.railway_traffic import track_axis as ta
from actions.railway_traffic import load_model_base as lmb
    
def get_traction_force(Lab:float):
    ''' Return the traction force according to expression (6.20) of 
        Eurocode 1 part 2 (clause 6.5.3 paragraph 2).

    :param Lab: influence length of the loaded part of track on the 
               bridge, which is most unfavourable for the design of the 
               structural element under consideration (m).
    '''
    return min(33e3*Lab, 1000e3)

def get_braking_force(Lab:float, loadModel:str):
    ''' Return the braking force according to expressions (6.21) and (6.22) of 
        Eurocode 1 part 2 (clause 6.5.3 paragraph 2).

    :param Lab: influence length of the loaded part of track on the 
               bridge, which is most unfavourable for the design of the 
               structural element under consideration (m).
    :param loadModel: string identifier for the load model (71, SW/O, SW/2 or HSLM).
    '''
    if(loadModel=='SW/2'):
        retval= 35e3
    elif(loadModel in ['71', 'SW/O', 'HSLM']):
        retval= min(20e3*Lab, 6000e3)
    else:
        methodName= sys._getframe(0).f_code.co_name
        errMsg= methodName+"; unknown load model: '"+str(loadModel)
        errMsg+= "'; known load models are: '71', 'SW/O', 'SW/2' and 'HSLM'."
        lmsg.error(errMsg)
    return retval

def centrifugal_force_reduction_factor(v, Lf):
    '''Returns the reduction factor of the centrifugal force according to 
       expression (6.19) of Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

    :param v: speed (m/s).
    :param Lf: influence length of the loaded part of curved track on the 
               bridge, which is most unfavourable for the design of the 
               structural element under consideration (m).
    '''
    vkmh= v*3.6
    if(vkmh<120):
        coefF= 1
    elif(vkmh<300):
        coefF= 1-(vkmh-120)/1000*(814/vkmh+1.75)*(1-math.sqrt(2.88/Lf)) 
    else:
       coefF= 0.197+0.803*(math.sqrt(2.88/Lf))
    coefF= max(coefF,0.35)
    return coefF

def get_centrifugal_forces(v, Lf, r, Qvk, qvk):
    ''' Compute the characteristic values of the concentrated (Qtk) and 
        distributed (qtk) centrifugal forces according to equations (6.17) and
        (6.18) of  Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

    :param v: speed (m/s).
    :param Lf: influence length of the loaded part of curved track on the 
               bridge, which is most unfavourable for the design of the 
               structural element under consideration (m).
    :param r: radius of curvature (m).
    :param Qvk: characteristic value of the vertical concentrated load 
                specified in clause 6.3 (excluding any enhancement for 
                dynamic effects).
    :param qvk: characteristic value of the vertical distributed load 
                specified in clause 6.3 (excluding any enhancement for 
                dynamic effects).
    '''
    ff= v*v/constants.g/r*centrifugal_force_reduction_factor(v= v, Lf= Lf)
    Qtk= ff*Qvk
    qtk= ff*qvk
    return (Qtk, qtk)

class LocomotiveLoad(ll.LocomotiveLoad):
    ''' Locomotive load as defined in EC-1.'''
    def __init__(self, nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435, dynamicFactor= 1.0, classificationFactor= 1.21):
        ''' Constructor.

        :param nAxes: defaults to 4 (Eurocode 1, load model 1)
        :param axleLoad: axle load.
        :param xSpacing: tandem axle spacing (defaults to 1.6 m, Eurocode 1, load model 1).
        :param ySpacing: distance between wheels of the same axle (defaults to nternational standard gauge 1435 mm).
        :param dynamicFactor: dynamic factor.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        super().__init__(nAxes= nAxes, axleLoad= axleLoad, xSpacing= xSpacing, ySpacing= ySpacing, dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)

    def getCentrifugalLoadPerWheel(self, v, Lf, r):
        ''' Compute the characteristic values of the concentrated (Qtk) and 
            distributed (qtk) centrifugal forces according to equations (6.17)
            and (6.18) of  Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
        '''
        ff= v*v/constants.g/r*centrifugal_force_reduction_factor(v= v, Lf= Lf)
        classifiedWheelLoad= self.getClassifiedWheelLoad()
        return ff*classifiedWheelLoad
                                   
    def getCentrifugalWheelLoads(self, v, Lf, r, trackCrossSection= None, h= 1.8):
        ''' Compute the characteristic values of the concentrated (Qtk) and 
            distributed (qtk) centrifugal forces according to equations (6.17)
            and (6.18) of  Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
        :param trackCrossSection: object that defines the cant and the gauge of the track (see TrackCrossSection class).
        :param h: height of the locomotive center of gravity.
        '''
        centrifugalWheelLoad= self.getCentrifugalLoadPerWheel(v= v, Lf= Lf, r= r)
        if(trackCrossSection):
            # Distribute the load
            Q= [2*centrifugalWheelLoad,0,0] # Centrifugal load.
            railLoads= trackCrossSection.getRailLoads(Q= Q, h= h)
            leftRailLoad= geom.Vector2d(railLoads[0], railLoads[1])
            rightRailLoad= geom.Vector2d(railLoads[2], railLoads[3])
            wheelPositions= self.getWheelPositions()
            retval= list()
            for wp in wheelPositions:
                if(wp.y<0): # right rail
                    retval.append(rightRailLoad)
                else:
                    retval.append(leftRailLoad)
        else:
            retval= (self.getNumWheels())*[centrifugalWheelLoad]
        return retval
    
    def getNosingLoad(self):
        ''' Return the value of the nosing load.'''
        return 100e3*self.classificationFactor # No dynamic factor for nosing load.


locomotiveLM1= LocomotiveLoad(nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435)

class TrainLoadModel(tlm.TrainLoadModel):
    ''' Model for the loads of a according to EC-1.'''
    
    def getCentrifugalWheelLoads(self, v, Lf, r, trackCrossSection):
        ''' Compute the characteristic values of the concentrated (Qtk) and 
            distributed (qtk) centrifugal forces according to equations (6.17)
            and (6.18) of  Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
        :param trackCrossSection: object that defines the cant and the gauge of the track (see TrackCrossSection class).
        '''
        return self.locomotive.getCentrifugalWheelLoads(v= v, Lf= Lf, r= r, trackCrossSection= trackCrossSection, h= self.h)

    def getCentrifugalLoadPerMeter(self, v, Lf, r, trackCrossSection):
        ''' Compute the characteristic values of the concentrated (Qtk) and 
            distributed (qtk) centrifugal forces according to equations (6.17)
            and (6.18) of  Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
        :param trackCrossSection: object that defines the cant and the gauge of the track (see TrackCrossSection class).
        '''
        ff= v*v/constants.g/r*centrifugal_force_reduction_factor(v= v, Lf= Lf)
        classifiedUniformLoad= self.getClassifiedUniformLoad()
        q= ff*classifiedUniformLoad
        if(trackCrossSection):
            # Distribute the load
            Q= [q,0,0] # Centrifugal load.
            railLoads= trackCrossSection.getRailLoads(Q= Q, h= self.h)
            leftRailLoad= geom.Vector2d(railLoads[0], railLoads[1])
            rightRailLoad= geom.Vector2d(railLoads[2], railLoads[3])
            retval= (leftRailLoad, rightRailLoad)
        else:
            retval= (q/2, q/2)
        return retval
            
    def getUniformLineLoadForDesignSituationII(self, bridgeLength= 20.0):
        ''' Return the uniformly distributed equivalent line load for design
            situation II according to paragraph (4) of clause 6.7.1 of 
            Eurocode 1 part 2.

        :param bridgeLength: length of the bridge.
        '''
        bridgeLength= min(bridgeLength, 20.0) # 20 m maximum.
        locomotiveLoad= self.locomotive.getTotalLoad()
        locomotiveLength= self.locomotive.getTotalLength()
        remainingLength= bridgeLength-locomotiveLength
        uniformLoad= (locomotiveLoad+self.getDynamicUniformLoad()*remainingLength)/bridgeLength
        return 1.4*uniformLoad

class TrackAxis(ta.TrackAxis):
    ''' Track axis according to EC-1.'''
    
    def getRailCentrifugalLoads(self, leftRailCentrifugalLoad, rightRailCentrifugalLoad, trainModel, relativePosition):
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
        dynamicFactor= 1.0 # no dynamic factor for centrifugal loads in EC-1.
        return super().getRailCentrifugalLoads(leftRailCentrifugalLoad, rightRailCentrifugalLoad, trainModel, relativePosition, overrideDynamicFactor= dynamicFactor)
    
    def defDeckCentrifugalLoadOnRailsThroughLayers(self, trainModel, relativePosition, v, Lf, r, trackCrossSection, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define the nodal loads that correspond to uniform centrifugal
           loads on the rails.

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
        :param trackCrossSection: object that defines the cant and the gauge of the track (see TrackCrossSection class).
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
        # Uniform centrifugal load on the rails.
        railCentrifugalLoadsPerMeter= trainModel.getCentrifugalLoadPerMeter(v= v, Lf= Lf, r= r, trackCrossSection= trackCrossSection)
        # Create the centrifugal rail loads.
        railCentrifugalLoads= self.getRailCentrifugalLoads(leftRailCentrifugalLoad= railCentrifugalLoadsPerMeter[0], rightRailCentrifugalLoad= railCentrifugalLoadsPerMeter[1], trainModel= trainModel, relativePosition= relativePosition)
        retval= list()
        # Apply loads to the originSet nodes.
        deckMidplane= originSet.nodes.getRegressionPlane(0.0)
        for rcl in railCentrifugalLoads:
            retval.extend(rcl.defDeckRailLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckMidplane= deckMidplane, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
    
    def defDeckCentrifugalLoadThroughLayers(self, trainModel, relativePosition, v, Lf, r, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define centrifugal loads on the bridge deck given:

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: relative positions of the locomotive center in
                                  the track axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param v: speed (m/s).
        :param Lf: influence length of the loaded part of curved track on the 
                   bridge, which is most unfavourable for the design of the 
                   structural element under consideration (m).
        :param r: radius of curvature (m).
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
        # Locomotive centrifugal load.
        trackCrossSection= self.getTrackCrossSection()
        locomotiveCentrifugalLoads= trainModel.getCentrifugalWheelLoads(v= v, Lf= Lf, r= r, trackCrossSection= trackCrossSection)
        ref= self.getReferenceAt(lmbdArcLength= relativePosition)
        retval= trainModel.locomotive.defDeckCentrifugalWheelLoadsThroughLayers(centrifugalLoads= locomotiveCentrifugalLoads, centrifugalDirection= -ref.getJVector(), ref= ref, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        # Uniform centrifugal load on the rails.
        retval.extend(self.defDeckCentrifugalLoadOnRailsThroughLayers(trainModel= trainModel, relativePosition= relativePosition, v= v, Lf= Lf, r= r, trackCrossSection= trackCrossSection, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
    
    def defDeckNosingLoadThroughLayers(self, trainModel, relativePosition:float, spreadingLayers, originSet, deckThickness, deckSpreadingRatio= 1/1):
        ''' Define loads on the bridge deck due to the nosing load given:

        :param trainModel: load model of the train (see TrainLoadModel class).
        :param relativePosition: relative positions of the locomotive center in
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
        '''
        # Locomotive centrifugal load.
        ref= self.getReferenceAt(lmbdArcLength= relativePosition)
        retval= trainModel.locomotive.defDeckNosingLoadThroughLayers(nosingDirection= -ref.getJVector(), ref= ref, spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio)
        return retval
    
