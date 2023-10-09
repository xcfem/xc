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
from misc_utils import log_messages as lmsg
from actions.railway_traffic import load_model_base as lmb

class LocomotiveLoad(lmb.DynamicFactorLoad):
    ''' Locomotive load.

    :ivar nAxes: number of axes.
    :ivar axleLoad: axle load.
    :ivar xSpacing: tandem axle spacing.
    :ivar yXpacing: distance between wheels of the same axle.
    '''
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
        super().__init__(dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.nAxes= nAxes
        self.axleLoad= axleLoad
        self.xSpacing= xSpacing
        self.ySpacing= ySpacing

    def getAxleClassifiedLoad(self):
        ''' Return the value of the axle load affected by the classification
            factor.'''
        return self.axleLoad*self.classificationFactor

    def getTotalLoad(self):
        ''' Return the total load of the tandem.'''
        return self.nAxes*self.getAxleClassifiedLoad()*self.dynamicFactor

    def getTotalLength(self):
        ''' Return the length occupied by the locomotive.'''
        return self.nAxes*self.xSpacing

    def getNumWheels(self):
        ''' Return the number of wheels of the locomotive.'''
        return self.nAxes*2

    def getWheelPositions(self):
        ''' Return a list with the positions of the wheels.

        :param swapAxes: if true swap X and Y axis.
        '''
        dX= self.xSpacing
        dY= self.ySpacing/2.0
        axesDisp= (self.nAxes-1)*dX/2.0  # Axes positions relative to the center.
        axlesPos= list()
        for i in range(0,self.nAxes):
            axlesPos.append(i*dX-axesDisp)
        retval= list()
        for y in [-dY, dY]:
            for x in axlesPos:
                retval.append(geom.Pos2d(x,y))
        return retval

    def getClassifiedWheelLoad(self):
        ''' Return the load on each wheel affected by the classification factor.'''
        return self.getAxleClassifiedLoad()/2.0

    def getDynamicWheelLoad(self):
        ''' Return the load on each wheel affected by the dynamic factor.'''
        return self.getClassifiedWheelLoad()*self.dynamicFactor

    def getWheelLoads(self, ref= None, loadFactor= 1.0, static= False):
        ''' Return the loads of the wheels of the tandem along with its
            positions.

        :param ref: reference system at the center of the locomotive.
        :param loadFactor: factor to apply to the loads.
        :param static: if true don't apply the dynamic factor.
        '''
        positions= self.getWheelPositions()
        if(static):
            wheelLoad= self.getClassifiedWheelLoad()*loadFactor
        else:
            wheelLoad= self.getDynamicWheelLoad()*loadFactor
        # Compute local positions.
        wheelLoads= list()
        for p in positions:
            wl= lmb.WheelLoad(pos= p, ld=wheelLoad)
            wheelLoads.append(wl)
        if(ref): # return the loads in global coordinates.
            retval= list()
            for wl in wheelLoads:
                pos2d= wl.position
                pos3d= ref.getGlobalPosition(pos2d)
                wl.position= pos3d
                wl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector())
                retval.append(wl)
        else: # return the loads in local coordinates.
            retval= wheelLoads 
        return retval
