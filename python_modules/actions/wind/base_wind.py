# -*- coding: utf-8 -*-
'''Base classes for wind load calculations '''
from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (A_OO) "
__copyright__= "Copyright 2020,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com"

import math
from enum import Enum
from misc_utils import log_messages as lmsg

class windSurfaceOrientation(Enum):
    windward= 0
    side= 1
    leeward= 2

class windParams(object):
    '''Basic wind parameters to define wind load.

    :ivar v: basic speed wind [m/s]
    :ivar Kd: wind directionality factor
    :ivar Kzt: topographic factor
    :ivar I: importance factor
    :ivar alpha: terrain exposure constant (according to exposure category)
    :ivar zg: terrain exposure constant (according to exposure category)
    '''

    def __init__(self,v,Kd,Kzt,I,alpha,zg):
        self.v=v
        self.Kd=Kd
        self.Kzt=Kzt
        self.I=I
        self.alpha=alpha
        self.zg=zg

    def Kz(self,z):
        '''velocity pressure exposure coefficient evaluated at height z above ground level 

        :param z: height of the point above the ground level [m]
        '''
        if z < 5:
            Kz=2.01*(5/self.zg)**(2/self.alpha)
        elif z <= self.zg:
            Kz=2.01*(z/self.zg)**(2/self.alpha)
        else:
            lmsg.warning("Can't evaluate Kz for z= "+ str(z) + " > zg= "+ str(zg))
        return Kz

    def qz(self,z):
        '''Velocity wind pressure evaluated at height z above ground level [N/m2]

        :param z: height of the point above the ground level [m]
        '''
        Kz=self.Kz(z)
        qz=0.613*Kz*self.Kzt*self.Kd*(self.v)**2*self.I
        return qz
        
                         

class cylindrWind(object):
    '''Distribution of wind pressures in the cylindrical body and roof of a tank (according to CIRSOC 102). Reference article: 'Efectos de la modelación de presiones por viento en altura sobre la estabilidad de tanques de almacenamiento de fluidos' , by Carlos A. Burgos, Rossana C. Jacab y Luis A. Godoy.

    :ivar diam: diameter of the cylindrical tank
    :ivar height: height of the cylindrical tank
    :ivar windComp: components [x,y] of a vector in wind direction 
                    (defaults to [1,0] -> wind in X direction)
    :ivar windParams: instance of windParams class defining the basic parameters of the wind
    :ivar zGround: ground level (defaults to 0)
    :ivar xCent: x-coordinate of the cylinder axis (defaults to 0)
    :ivar yCent: y-coordinate of the cylinder axis (defaults to 0)
    '''

    def __init__(self,diam,height,windParams,windComp=[1,0],zGround=0,xCent=0,yCent=0):
        self.diam=diam
        self.height=height
        self.windParams=windParams
        self.windComp=windComp
        self.zGround=zGround
        self.xCent=xCent
        self.yCent=yCent
        self.vectRef=geom.Vector2d(-windComp[0],-windComp[1])
        
    def Cp1_theta(self,theta):
        '''Circumferential variation of the pressure coefficient

        - Cp1_theta (+) -> pressure acts toward external surface
        - Cp1_theta (-) -> pressure acts away from external surface

        :param theta: angle measured from the windward meridian passing through the centre of the cylinder [radians]
        '''
        Cp1_theta=-0.5+0.4*math.cos(theta)+0.8*math.cos(2*theta)+0.3*math.cos(3*theta)-0.1*math.cos(4*theta)-0.05*math.cos(5*theta)
        return Cp1_theta

    def kc(self,Cp1):
        '''Coefficient depending on the selenderness (H/D) of the tank

        :param Cp1: Circumferential variation of the pressure coefficient
        '''
        if Cp1 >= -0.15:
            kc=1.0
        else:
            kc=1.0-0.55*(Cp1+0.15)*math.log10(self.height/self.diam)
        return kc
    
    def Cp(self,x,y):
        '''External wind pressure coefficient in a point of the cylinder 
        with coordinates (x,y) 

        - Cp (+) -> pressure acts toward external surface
        - Cp (-) -> pressure acts away from external surface

        '''
        vectPoint=geom.Vector2d(x-self.xCent,y-self.yCent)
        theta=self.vectRef.getAngle(vectPoint)
        Cp1=self.Cp1_theta(theta)
        kc=self.kc(Cp1)
        Cp=kc*Cp1
        return Cp
        
    def getWindPress(self,x,y,z):
        '''External wind pressure in a point of the cylinder with 
        coordinates (x,y,z)

        - Wpress (+) -> pressure acts toward external surface
        - Wpress (-) -> pressure acts away from external surface

        '''
        zCalc=z-self.zGround
        qz=self.windParams.qz(zCalc)
        Cp=self.Cp(x,y)
        Wpress=qz*Cp
        return Wpress
    
