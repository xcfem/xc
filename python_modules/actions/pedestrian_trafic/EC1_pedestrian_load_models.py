# -*- coding: utf-8 -*-
''' Functions to model pedestrian loads according to prEN1991-2 
   (Draft stage 34) – General actions – Traffic loads on bridges; 
   Annex X (informative) Dynamic models of pedestrian loads. 
   (August 2001).'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2021, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"

import math
import sys

# Individual load model.

## References
#  [1] Rebelo, Carlos & Negrao, J.H.J.O. & Rigueiro, Constança. (2005). Numerical simulation of timber footbridges dynamic response under pedestrian loads.
#  [2] Bachmann, H. e Ammann, W (1987) Vibrations in Structures Induced by Man and Machines, Journal of IABSE, Structural Engineering International, vol 5

defaultPhaseAngles= [0.0, math.pi/2.0, math.pi/2.0] # reference [2]

def getFourierCoefficients(walking:bool, fs, triangularModel= False):
    ''' Return the values of the Fourier coefficients according to table 1
        of the reference [1].

    :param walking: true if the pedestrian is supposed to walk.
    :param fs: step frequency.
    :param triangularModel: if true use a triangular model in the time domain
                            otherwise use a half-sine model.
    '''
    retval= [0.1,0.1,0.1]
    if(walking):
        if(1<fs and fs<=2.5):
            retval[0]= 0.43*fs-0.38
            if(fs>1.5):
                retval[1]= 0.15*fs-0.125
        else:
            funcName= sys._getframe(0).f_code.co_name
            lmsg.warning(funcName+'; frequency: '+str(fs)+' out of range [1,2.5] Hz. Is the pedestrian walking?')
    else: # running.
        if(fs<2 or fs>4):
            funcName= sys._getframe(0).f_code.co_name
            lmsg.warning(funcName+'; frequency: '+str(fs)+' out of range [2,4] Hz. Is the pedestrian running?')
        else:
            tc= 1.8*math.pow(fs,-2.05) # foot-to-deck contact period (Fig. 2 ref. [1])
            for i in [1,2,3]:
                tmp= i*math.pi*fs*tc
                c= math.cos(tmp)
                d= tmp**2
                if(triangularModel):
                    retval[i-1]= 4*(1-c)/d
                else:
                    retval[i-1]= 2*(1-c)/(1-d)
    return retval

def getVerticalPedestrianLoad(t:float, fs:float, fourierCoefficients, phaseAngles= defaultPhaseAngles, G= 700.0):
    ''' Return the vertical load of a single pedestrian 
        according to expression (2) in reference [1].

    :param t: time for which the load will be calculated.
    :param fs: step frequency.
    :param fourierCoefficients: values of the coefficients to use in the
                                computation of the Fourier series.
    :param phaseAngles: phase angles with respect to the first harmonic
                        in the Fourier series.
    :param G: weight of the reference pedestrian (defaults to 700 N).
    '''
    retval= 1.0 # vertical load.
    for i, (alpha_i, phi_i) in enumerate(zip(fourierCoefficients, phaseAngles)):
        retval+= alpha_i*math.sin(2*math.pi*i*fs*t-phi_i)
    retval*=G
    return retval

def getHorizontalPedestrianLoad(t:float, fs:float, fourierCoefficients, phaseAngles= defaultPhaseAngles, G= 700.0):
    ''' Return the horizontal load of a single pedestrian 
        according to expression (2) in reference [1].

    :param t: time for which the load will be calculated.
    :param fs: step frequency.
    :param fourierCoefficients: values of the coefficients to use in the
                                computation of the Fourier series.
    :param phaseAngles: phase angles with respect to the first harmonic
                        in the Fourier series.
    :param G: weight of the reference pedestrian (defaults to 700 N).
    '''
    retval= 0.0 # horizontal load.
    for i, (alpha_i, phi_i) in enumerate(zip(fourierCoefficients, phaseAngles)):
        retval+= alpha_i*math.sin(math.pi*i*fs*t-phi_i) # half the frequency of the horizontal load.
    retval*=G
    return retval

class PedestrianLoad(object):
    ''' Pedestrian load model according to prEN1991-2 
       (Draft stage 34) – General actions – Traffic loads on bridges; 
       Annex X (informative) Dynamic models of pedestrian loads. 
       (August 2001).

    :ivar fs: step frequency.
    :ivar G: weight of the reference pedestrian (defaults to 700 N).
    :ivar fourierCoefficients: values of the coefficients to use in the
                               computation of the Fourier series.
    :ivar phaseAngles: phase angles with respect to the first harmonic
                      in the Fourier series.
    '''
    def __init__(self, fs:float, walking:bool, G= 700.0, triangularModel= False, phaseAngles= defaultPhaseAngles):
        ''' Constructor.

        :param fs: step frequency.
        :param walking: true if the pedestrian is supposed to walk.
        :param G: weight of the reference pedestrian (defaults to 700 N).
        :param triangularModel: if true use a triangular model in the time 
                                domain otherwise use a half-sine model.
        :param phaseAngles: phase angles with respect to the first harmonic
                            in the Fourier series.
        '''
        self.fs= fs
        self.G= G
        self.fourierCoefficients= getFourierCoefficients(walking= walking, fs= fs, triangularModel= False)
        self.phaseAngles= phaseAngles

    def getVerticalLoad(self, t):
        ''' Return the vertical load of a single pedestrian.

        :param t: time for which the load will be calculated.
        '''
        return getVerticalPedestrianLoad(t= t,fs= self.fs, fourierCoefficients= self.fourierCoefficients)
    
    def getHorizontalLoad(self, t):
        ''' Return the horizontal load of a single pedestrian.

        :param t: time for which the load will be calculated.
        '''
        return getHorizontalPedestrianLoad(t= t,fs= self.fs, fourierCoefficients= self.fourierCoefficients)


