# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com "

#Rough calculation for the analysis of T-beam bridges

def loadDistrCourbon(P,e,distGirdAx):
    '''Estimates the distribution of the live loads among the longitudinal
    girders according to the Courbon's theory. In this method the effect of 
    the variation of span is not at all considered. All the longitudinal
    beams are considered identical.

    :param P: total live load
    :param e: eccentricity (+ or -) from the axis of the bridge of the live load (or COG of loads in case of multiple loads)
    :param n: number of longitudinal girders
    :param distGirdAx: list with the distances (+ or -)of all the girders from the axis of the bridge

    return list of reactions for each of the longitudinal girders
    '''
    retval=[]
    n=len(distGirdAx)
    d=sum(i*i for i in distGirdAx)
    for i in distGirdAx:
        retval.append(P/n*(1+n*i*e/d))
    return retval
