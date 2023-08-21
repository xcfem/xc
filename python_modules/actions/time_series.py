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

