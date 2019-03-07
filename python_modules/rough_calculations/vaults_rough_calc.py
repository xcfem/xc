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
    

class hyparUmbrella(object):
    '''Rough calculation of a hyperbolic paraboloid umbrella
    :ivar lx: size of one tympan (one-fourth of the umbrella) in X direction
    :ivar ly: size of one tympan (one-fourth of the umbrella) in Y direction
    :ivar zMax: Z coordinate of the umbrella corner
    :ivar zSupp: Z coordinate of the umbrella central point (on the support)
    :ivar thickness: thickness of the shell
    :ivar k: warping of the hyperbolic paraboloid
    '''
    
    def __init__(self,lx,ly,zMax,zSupp,thickness):
        self.lx=lx
        self.ly=ly
        self.zMax=zMax
        self.zSupp=zSupp
        self.k=(self.zMax-self.zSupp)/(self.lx*self.ly)
        self.thickness=thickness

    def getZ(self,x,y):
        '''return Z coordinate of point (x,y)
        '''
        return self.k*x*y

    def getLedge(self):
        '''return the maximum length of half an umbrella edge'''
        return max(self.lx,self.ly)

    def getLvalley(self):
        '''return the length of the valley in the umbrella
        '''
        Ledge=self.getLedge()
        c0=self.zMax-self.zSupp
        return math.sqrt(Ledge**2+c0**2)

    def getNxy(self,pz):
        '''return shearing force in the hypar shell

        :param pz: vertical load per unit area of ground projection
        '''
        return -pz/2/self.k

    def getCompStress(self,pz):
        '''return the maximum compressive stress in the hypar

        :param pz: vertical load per unit area of ground projection
        '''
        return -pz/2/self.k/self.thickness
    
    def getTensStress(self,pz):
        '''return the maximum tensile stress in the hypar

        :param pz: vertical load per unit area of ground projection
        '''
        return pz/2/self.k/self.thickness
    
    def getTmax(self,pz):
        '''return the maximum tension force in edge of umbrella

        :param pz: vertical load per unit area of ground projection
        '''
        Nxy=self.getNxy(pz)
        Ledge=self.getLedge()
        return -Nxy*Ledge


    def getCmax(self,pz):
        '''return the maximum compression force in edge of umbrella

        :param pz: vertical load per unit area of ground projection
        '''
        Nxy=self.getNxy(pz)
        Lvalley=self.getLvalley()
        return 2*Nxy*Lvalley

        
        
