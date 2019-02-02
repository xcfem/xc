# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com "

# based on the course 'The art of structural engineering : Vaults',
# by Maria Garlock

import math

class SphericalDome(object):
    '''Rough calculation of a spherical dome

    :ivar sphRadius: radius of curvature (spherical radius)
    :ivar alpha: angle from the top of the dome to the bottom of the dome
          measured along the meridian [radians]
    '''
    def __init__(self,sphRadius,alpha):
        self.sphRadius=sphRadius
        self.alpha=alpha

    def getPlanRadius(self):
        '''Return the planar radius of the dome at the base'''
        return self.sphRadius*math.sin(self.alpha)

    def getSpan(self):
        '''Return the span of the dome'''
        return 2*self.getPlanRadius()

    def getRise(self):
        '''Return the height (or rise) of the dome'''
        return (self.sphRadius-self.sphRadius*math.cos(self.alpha))
    
    def getSurfArea(self):
        '''Return the surface area of the dome'''
        return 2*math.pi*self.sphRadius*self.getRise()

    def getVerticalSupportReaction(self,qVsurf):
        '''Return the vertical linear reaction at the base of the dome due 
        to a uniform surface load qVsurf over the dome'''
        VR=qVsurf*self.getSurfArea()/(2*math.pi*self.getPlanRadius())
        return VR

    def getMeridionalSupportReaction(self,qVsurf):
        '''Return the meridional linear reaction at the base of the dome due 
        to a uniform surface load qVsurf over the dome'''
        NR=self.getVerticalSupportReaction(qVsurf)/math.sin(self.alpha)
        return NR
        
    def getHorizSupportReaction(self,qVsurf):
        '''Return the horizontal linear reaction at the base of the dome due 
        to a uniform surface load qVsurf over the dome'''
        HR=self.getMeridionalSupportReaction(qVsurf)*math.cos(self.alpha)
        return HR

    def getTensionForceRing(self,qVsurf):
        '''Return the tension force in the base ring due
        to a uniform surface load qVsurf over the dome'''
        T=self.getHorizSupportReaction(qVsurf)*self.getPlanRadius()
        return T
    
        
