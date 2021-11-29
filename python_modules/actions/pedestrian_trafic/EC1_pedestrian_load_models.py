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
from misc_utils import log_messages as lmsg
import matplotlib.pyplot as plt

# Individual load model.

## References
#  [1] Rebelo, Carlos & Negrao, J.H.J.O. & Rigueiro, Constança. (2005). Numerical simulation of timber footbridges dynamic response under pedestrian loads.
#  [2] Bachmann, H. e Ammann, W (1987) Vibrations in Structures Induced by Man and Machines, Journal of IABSE, Structural Engineering International, vol 5
#  [3] Footbridges. Assessment of vibrational behaviour of footbridges under pedestrian loading. Technical guide. Service d'Études techniques des routes et autoroutes. October 2006.

defaultPhaseAngles= [0.0, math.pi/2.0, math.pi/2.0] # references [2] and [3]

def getFourierCoefficients(walking:bool, fs, triangularModel= False):
    ''' Return the values of the Fourier coefficients according to table 1
        of the reference [1].

    :param walking: true if the pedestrian is supposed to walk.
    :param fs: step frequency.
    :param triangularModel: if true use a triangular model in the time domain
                            otherwise use a half-sine model.
    '''
    lmsg.warning('This function returns very high values of the fourier coefficients. Use with caution.')
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

setraVerticalFourierCoefficients= [0.4, 0.1, 0.1] # section 3.2.1 reference [3]
setraLateralFourierCoefficients= [0.05, 0.0, 0.0] # section 3.2.1 reference [3]
defaultVerticalFourierCoefficients= setraVerticalFourierCoefficients
defaultLateralFourierCoefficients= setraLateralFourierCoefficients

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
    factor= 2*math.pi*fs*t
    for i, (alpha_i, phi_i) in enumerate(zip(fourierCoefficients, phaseAngles)):
        if(alpha_i!=0.0):
            retval+= alpha_i*math.sin(factor*(i+1)-phi_i)
    retval*=G
    return retval

def getLateralPedestrianLoad(t:float, fs:float, fourierCoefficients, phaseAngles= defaultPhaseAngles, G= 700.0):
    ''' Return the lateral load of a single pedestrian 
        according to expression (2) in reference [1].

    :param t: time for which the load will be calculated.
    :param fs: step frequency.
    :param fourierCoefficients: values of the coefficients to use in the
                                computation of the Fourier series.
    :param phaseAngles: phase angles with respect to the first harmonic
                        in the Fourier series.
    :param G: weight of the reference pedestrian (defaults to 700 N).
    '''
    retval= 0.0 # lateral load.
    factor= math.pi*fs*t
    for i, (alpha_i, phi_i) in enumerate(zip(fourierCoefficients, phaseAngles)):
        if(alpha_i!=0.0):
            retval+= alpha_i*math.sin(factor*(i+1)-phi_i) # half the frequency of the lateral load.
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
    def __init__(self, fs:float, walking:bool, G= 700.0, verticalFourierCoefficients= defaultVerticalFourierCoefficients, lateralFourierCoefficients= defaultLateralFourierCoefficients, phaseAngles= defaultPhaseAngles):
        ''' Constructor.

        :param fs: step frequency.
        :param walking: true if the pedestrian is supposed to walk.
        :param G: weight of the reference pedestrian (defaults to 700 N).
        :param verticalFourierCoefficients: Fourier coefficients for vertical force.
        :param lateralFourierCoefficients: Fourier coefficients for lateral force.
        :param triangularModel: if true use a triangular model in the time 
                                domain otherwise use a half-sine model.
        :param phaseAngles: phase angles with respect to the first harmonic
                            in the Fourier series.
        '''
        self.fs= fs
        self.G= G
        self.verticalFourierCoefficients= verticalFourierCoefficients
        self.lateralFourierCoefficients= lateralFourierCoefficients
        self.phaseAngles= phaseAngles

    def getVerticalLoad(self, t):
        ''' Return the vertical load of a single pedestrian.

        :param t: time for which the load will be calculated.
        '''
        return getVerticalPedestrianLoad(t= t,fs= self.fs, fourierCoefficients= self.verticalFourierCoefficients)
    
    def getLateralLoad(self, t):
        ''' Return the lateral load of a single pedestrian.

        :param t: time for which the load will be calculated.
        '''
        return getLateralPedestrianLoad(t= t,fs= self.fs, fourierCoefficients= self.lateralFourierCoefficients)

    def getTimeSequence(self, tStart= 0.0, tInc= None, duration= None):
        ''' Return a time sequence.

        :param tStart: first item in the sequence.
        :param tInc: time increment (defaults to T/10).
        :param duration: duration of the sequence (defaults to 2*T).
        '''
        retval= list()
        T= 1.0/self.fs
        if(duration==None):
            duration= 4.0*T
        if(tInc==None):
            tInc= T/10.0
        numSteps= int(duration/tInc)+1
        t= tStart
        for i in range(0,numSteps):
            retval.append(t)
            t+= tInc
        return retval

    def getVerticalLoadHistory(self, ti):
        ''' Return the vertical loads for each item of the input list.

        :param ti: time values.
        '''
        retval= list()
        for t in ti:
            retval.append(self.getVerticalLoad(t))
        return retval

    def getLateralLoadHistory(self, ti):
        ''' Return the lateral loads for each item of the input list.

        :param ti: time values.
        '''
        retval= list()
        for t in ti:
            retval.append(self.getLateralLoad(t))
        return retval

    def plot(self, loadToDisp:str, tStart= 0.0, tInc= None, duration= None):
        ''' Create a plot of the loads.

        :parma loadToDisp: load to display (vertical, lateral or both).
        :param tStart: first item in the sequence.
        :param tInc: time increment (defaults to T/10).
        :param duration: duration of the sequence (defaults to 2*T).
        '''
        ti= self.getTimeSequence(tStart= tStart, tInc= tInc, duration= duration)
        vLoad= None
        if(loadToDisp=='vertical' or loadToDisp=='both'):
            vLoad= self.getVerticalLoadHistory(ti) # Vertical load.
        hLoad= None
        if(loadToDisp=='lateral' or loadToDisp=='both'):
            hLoad= self.getLateralLoadHistory(ti) # Lateral load.
        title= 'Single pedestrian. Vertical and lateral forces.'
        if(loadToDisp=='vertical'):
            title= 'Single pedestrian. Vertical force.'
        elif(loadToDisp=='lateral'):
            title= 'Single pedestrian. Lateral force.'
        plt.title(title)
        plt.xlabel('time (s)')
        plt.xlabel('force (N)')
        if(vLoad!=None):
            plt.plot(ti, vLoad, label='Vertical load')
        if(hLoad!=None):
            plt.plot(ti, hLoad, label='Lateral load')
        plt.legend()
        plt.show()


