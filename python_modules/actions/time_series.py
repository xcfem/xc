# -*- coding: utf-8 -*-
''' Time series related Python code.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2023, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import math
from misc_utils import log_messages as lmsg
import matplotlib.pyplot as plt

def def_trig_time_series(preprocessor, name, tStart:float, tEnd:float, period:float, cFactor:float= 1.0, shift:float= 0.0):
    ''' Return a sine or trigonometric time series that defines a sinusoidal 
        load factor λ(t) as a function of time. The defined time series returns
        cFactor*math.sin(2*math.pi(t-tStart)/period+shift) between tStart
        and tEnd and 0.0 outside this time window.

    :param preprocessor: Pre-processor of the FE problem at hand.
    :param name: name for the new time series.
    :param tStart: Start time of the non-zero load factor.
    :param tEnd: End time of the non-zero load factor.
    :param period: Characteristic period of the sine wave.
    :param cFactor: Amplification or peak factor multiplier (default: 1.0).
    :param shift: Phase shift in radians (default: 0.0)
    '''
    loadHandler= preprocessor.getLoadHandler
    lPatterns= loadHandler.getLoadPatterns
    retval= lPatterns.newTimeSeries("trig_ts","ts")
    retval.factor= cFactor
    retval.tStart= tStart
    retval.tFinish= tEnd
    retval.period= period
    retval.shift= shift
    return retval

def plot_time_series(timeSeries, timeIncrement= None, timeUnits= None):
    ''' Shows a diagram of the time series.

    :param name: name of the time series to display.
    :param timeIncrement: time increment to use in the diagram
                          if None then timeIncrement= duration/100.0
    '''
    duration= timeSeries.getDuration()
    tsType= timeSeries.type()
    if(duration==0):
        if(tsType in ['XC::ConstantSeries', 'XC::LinearSeries']):
            duration= 100 # Assign an arbitrary duration.
    if(timeIncrement is None):
        timeIncrement= duration/100.0
    numSteps= int(math.ceil(duration/timeIncrement))
    ti= list()
    vi= list()
    ## Compute  values.
    timeIncrement= duration/numSteps
    t= 0
    for i in range(0,numSteps):
        ti.append(t)
        vi.append(timeSeries.getFactor(t))
        t+= timeIncrement
    plt.plot(ti, vi, "-b")
    ## Add title and axis names
    plt.title('time series: '+ timeSeries.name)
    xLabel= 'time'
    if(timeUnits):
        xLabel+= ' ('+timeUnits+')'
    plt.xlabel(xLabel)
    plt.xticks(rotation = 90)
    plt.ylabel('factor')
    plt.grid()
    plt.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    plt.show()

