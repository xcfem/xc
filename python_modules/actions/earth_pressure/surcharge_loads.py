# -*- coding: utf-8 -*-
from __future__ import division
import math

''' Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) due to different kind of surcharge loads.
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

class StripLoadOnBackfill(object):
    '''Lateral earth pressure on a retaining wall due to a strip surcharge 
    load on the backfill. (J.Calavera, pg.40)

    :ivar qLoad: surcharge load (force per unit area)
    :ivar zLoad: global Z coordinate where the surcharge load acts
    :ivar distWall: minimal horizontal distance between the wall and the 
                    surcharge load
    :ivar WithStrip: with of the strip
    :ivar coef: is a coefficient = 1.5 (default) for the usual case of non-rigid
                walls. It can be redefined =2 for rigid walls
    '''
    def __init__(self,qLoad, zLoad,distWall, WithStrip):
        self.qLoad=qLoad
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.WithStrip=WithStrip
        self.coef=1.5
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z
        '''
        ret_press=0.0
        difZ=self.zLoad-z
        if difZ>0:
            bet1=math.atan(self.distWall/difZ)
            bet2=math.atan((self.distWall+self.WithStrip)/difZ)
            beta=bet2-bet1
            omega=bet1+beta/2.
            ret_press=self.coef*self.qLoad/math.pi*(beta-math.sin(beta)*math.cos(2*omega))
        print 'z,pres', z,',', ret_press
        return ret_press


class LineVerticalLoadOnBackfill(object):
    '''Lateral earth pressure on a retaining wall due to line surcharge 
    load acting in vertical direction on the backfill. (J.Calavera, pg.41)

    :ivar qLoad: surcharge load (force per unit length)
    :ivar zLoad: global Z coordinate where the line load acts
    :ivar distWall: horizontal distance between the wall and the line 
                    surcharge load
    '''
    def __init__(self,qLoad, zLoad,distWall):
        self.qLoad=qLoad
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z
        '''
        ret_press=0.0
        difZ=self.zLoad-z
        if difZ>0:
            omega=math.atan(self.distWall/difZ)
            ret_press=self.qLoad/math.pi/difZ*(math.sin(2*omega))**2
        print 'z,pres', z,',', ret_press
        return ret_press


class HorizontalLoadOnBackfill(object):
    '''Lateral earth pressure on a retaining wall due to a surcharge 
    load acting in horizontal direction on the backfill. 

    :ivar soilIntFi: agle of internal friction of the soil (º)
    :ivar qLoad: surcharge load (force per unit length)
    :ivar zLoad: global Z coordinate where the line load acts
    :ivar distWall: minimal horizontal distance between the wall and
          the area where the surcharge load acts (e.g.: a foundation)
    :ivar widthLoadArea: width (perperdicular to wall) of the area on
          which the horizontal load acts (e.g.: a foundation). 
    :ivar lengthLoadArea: width (parallel to wall) of the area on
          which the horizontal load acts (e.g.: a foundation). We can
          take lengthLoadArea=1 (default) for a continous load  
    :ivar horDistrAngle: angle to distribute load in the direction of
          the lengthLoadArea (defaults to 0 to apply all the load to a
          length of wall =lengthLoadArea, which would be the case of a
          continous load)

    '''
    def __init__(self,soilIntFi, qLoad, zLoad,distWall,widthLoadArea,lengthLoadArea=1,horDistrAngle=0):
        self.soilIntFi=soilIntFi
        self.qLoad=qLoad
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.widthLoadArea=widthLoadArea
        self.lengthLoadArea=lengthLoadArea
        self.horDistrAngle=horDistrAngle

    def setup(self):
        '''Calculate basic parameters
        '''
        IntFiRad=math.radians(self.soilIntFi)
        psi=math.pi/4+IntFiRad/2
        self.zpresmax=self.zLoad-self.distWall*math.tan(IntFiRad)
        self.zpresmin=self.zLoad-(self.distWall+self.widthLoadArea)*math.tan(psi)
        horDistrAngleRad=math.radians(self.horDistrAngle)
        self.presmax=(self.qLoad*self.lengthLoadArea)/(self.lengthLoadArea+2*self.distWall*math.tan(horDistrAngleRad))*2/(self.zpresmax-self.zpresmin)
                                                  
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z
        '''
        ret_press=0
        if z<=self.zpresmax and z>=self.zpresmin:
            ret_press=self.presmax/(self.zpresmax-self.zpresmin)*(z-self.zpresmin)
        print 'z,pres', z,',', ret_press
        return ret_press


                                                  
                                                  
