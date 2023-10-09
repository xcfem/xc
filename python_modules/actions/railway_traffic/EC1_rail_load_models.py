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
from actions.railway_traffic import load_model_base as lmb

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
        :param ignoreVerticalLoad: if true, consider
        '''
        ff= v*v/constants.g/r*centrifugal_force_reduction_factor(v= v, Lf= Lf)
        classifiedWheelLoad= self.getClassifiedWheelLoad()
        centrifugalWheelLoad= ff*classifiedWheelLoad
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


locomotiveLM1= LocomotiveLoad(nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435)

class TrackAxes(lmb.TrackAxes):
    ''' Notional lanes for a road section according to clause 4.2.3 
    of EC1-2:2003.

    '''
    def __init__(self, trackAxesPolylines):
        ''' Constructor.

        :param railAxesPolylines: 3D polylines defining the axis of track.
        '''
        super().__init__(trackAxesPolylines)

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
