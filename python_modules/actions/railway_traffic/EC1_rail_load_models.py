# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to Eurocode 1 part 2.'''

__author__= "Luis C. Pérez Tato (LP) "
__copyright__= "Copyright 2015, LP"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy import constants
from actions.railway_traffic import load_model_base as lmb

locomotiveLM1= lmb.LocomotiveLoad(nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435)

class TrackAxes(lmb.TrackAxes):
    ''' Notional lanes for a road section according to clause 4.2.3 
    of EC1-2:2003.

    '''
    def __init__(self, trackAxesPolylines):
        ''' Constructor.

        :param railAxesPolylines: 3D polylines defining the axis of track.
        '''
        super().__init__(trackAxesPolylines)

def centrifugal_force_reduction_factor(v, Lf):
    '''Returns the reduction factor of the centrifugal force according to 
       expression (6.19) of Eurocode 1 part 2 (clause 6.5.1 paragraph 8).

    :param v: speed (m/s).
    :param Lf: influence length of the loaded part of curved track on the 
               bridge, which is most unfavourable for the design of the 
               structural element under consideration (m).
    '''
    vkmhIAPF= v*3.6
    if(vkmhIAPF<120):
        coefFIAPF= 1
    elif(vkmhIAPF<300):
        coefFIAPF= 1-(vkmhIAPF-120)/1000*(814/vkmhIAPF+1.75)*(1-math.sqrt(2.88/Lf)) 
    else:
       coefFIAPF= 0.197+0.803*(math.sqrt(2.88/Lf))
    coefFIAPF= max(coefFIAPF,0.35)
    return coefFIAPF

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
