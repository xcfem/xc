# -*- coding: utf-8 -*-
from __future__ import division

'''Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) exerced by a given back soil with 
or without water level

Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) due to different kind of loads on the backfill.
'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import math
import xc_base
import geom
import xc
from miscUtils import LogMessages as lmsg

class PressureModelBase(object):
    '''Basse class for objects defining earth pressures.'''
    
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        lsmg.error('Error: getPressure must be overloaded in derived classes.')
        return 0.0

    def appendLoadToCurrentLoadPattern(self,xcSet,vDir,iCoo= 2,delta= 0.0):
        '''Append earth thrust on a set of elements to the current
        load pattern.

        :param xcSet: set that contains the elements (shells and/or beams)
        :param vDir: unit xc vector defining pressures direction
        :param iCoo: index of the coordinate that represents depth.
        :param delta: soil-wall friction angle (usually: delta= 2/3*Phi).
        '''
        tanDelta= math.tan(delta)
        tanVector= xc.Vector([-vDir[1],vDir[0]]) #iCoo= 1 => 2D
        if(iCoo==2): #3D
          tangVector= xc.Vector([vDir[2],vDir[1],-vDir[0]])
        if(len(vDir)==3): #3D load.
          for e in xcSet.getElements:
              presElem=self.getPressure(e.getCooCentroid(False)[iCoo])
              loadVector= presElem*(vDir+tanDelta*tanVector)
              if(presElem!=0.0):
                  e.vector3dUniformLoadGlobal(loadVector)
        else: #2D load.
          for e in xcSet.getElements:
              presElem=self.getPressure(e.getCooCentroid(False)[iCoo])
              loadVector= presElem*(vDir+tanDelta*tanVector)
              if(presElem!=0.0):
                  e.vector2dUniformLoadGlobal(loadVector)


class EarthPressureModel(PressureModelBase):
    '''Parameters to define a load of type earth pressure

      :ivar K:         coefficient of pressure
      :ivar zGround:   global Z coordinate of ground level
      :ivar gammaSoil: weight density of soil 
      :ivar zWater:    global Z coordinate of groundwater level 
                       (if zGroundwater<minimum z of model => there is no groundwater)
      :ivar gammaWater: weight density of water
    '''
    def __init__(self,K , zGround, gammaSoil, zWater, gammaWater):
        super(EarthPressureModel,self).__init__()
        self.K= K
        self.zGround= zGround
        self.gammaSoil= gammaSoil
        self.zWater= zWater
        self.gammaWater= gammaWater

    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        ret_press= 0.0
        if(z<self.zGround):
            if(z>self.zWater):
                ret_press=self.K*self.gammaSoil*(self.zGround-z)
            else:
                ret_press=self.K*(self.gammaSoil*(self.zGround-self.zWater) + (self.gammaSoil-self.gammaWater)*(self.zWater-z)) + self.gammaWater*(self.zWater-z)
        return ret_press

class PeckPressureEnvelope(EarthPressureModel):
    ''' Envelope of apparent lateral pressure diagrams for design 
        of cuts in sand. See 10.2 in the book "Principles of Foundation
        Engineering" from Braja M. Das.

      :ivar H:         height of the cut.
      :ivar phi:       effective friction angle of sand.
    '''
    def __init__(self,phi , zGround, gammaSoil, zWater, gammaWater, H):
        Ka= math.tan(math.pi/4.0-phi/2.0)**2 # Rankine active pressure coefficient.
        super(PeckPressureEnvelope,self).__init__(Ka , zGround, gammaSoil, zWater, gammaWater)
        self.H= H
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        ret_press= 0.0
        if(z<self.zGround):
            ret_press=0.65*self.K*self.gammaSoil*self.H
            if(z<self.zWater):
              lmsg.error('pressures under water table not implemented.''')
        return ret_press

class UniformLoadOnStem(PressureModelBase):
    '''Uniform lateral earth pressure on a retaining wall.

    :ivar qLoad: surcharge load (force per unit area)
    '''
    def __init__(self,qLoad):
        super(UniformLoadOnStem,self).__init__()
        self.qLoad=qLoad
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        return self.qLoad

class StripLoadOnBackfill(UniformLoadOnStem):
    '''Lateral earth pressure on a retaining wall due to a strip surcharge 
    load on the backfill. (J.Calavera, pg.40)

    :ivar zLoad: global Z coordinate where the surcharge load acts
    :ivar distWall: minimal horizontal distance between the wall and the 
                    surcharge load
    :ivar stripWidth: width of the strip
    :ivar coef: is a coefficient = 1.5 (default) for the usual case of non-rigid
                walls. It can be redefined =2 for rigid walls
    '''
    def __init__(self,qLoad, zLoad,distWall, stripWidth):
        super(StripLoadOnBackfill,self).__init__(qLoad)
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.stripWidth=stripWidth
        self.coef=1.5
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z
        '''
        ret_press=0.0
        difZ=self.zLoad-z
        if difZ>0:
            bet1=math.atan(self.distWall/difZ)
            bet2=math.atan((self.distWall+self.stripWidth)/difZ)
            beta=bet2-bet1
            omega=bet1+beta/2.
            ret_press=self.coef*self.qLoad/math.pi*(beta-math.sin(beta)*math.cos(2*omega))
        return ret_press

    def appendVerticalLoadToCurrentLoadPattern(self,xcSet,vDir,iXCoo= 0,iZCoo= 2,alph= math.radians(30)):
        '''Append to the current load pattern the vertical pressures on 
           a set of elements due to the strip load. According to
           11.3.4 in the book "Mecánica de suelos" of Llano, J.J.S.
           isbn= 9788471461650 (https://books.google.ch/books?id=oQFZRKlix\_EC)

        :param xcSet: set that contains the elements.
        :param vDir: unit xc vector defining pressures direction
        :param alph: angle of stress spreading.
        '''
        tanAlph= math.tan(alph)
        avgZCoo= 0.0
        length= 0.0
        for e in xcSet.getElements:
            z= e.getCooCentroid(False)[iZCoo]
            l= e.getLineSegment(False).getLongitud()
            avgZCoo+=z*l
            length+= l
        avgZCoo/= length
        H= self.zLoad-avgZCoo
        xMin= self.distWall-self.stripWidth-H*tanAlph
        xMax= self.distWall+self.stripWidth+H*tanAlph
        L= xMax-xMin
        sigma_v= self.qLoad*self.stripWidth/L
        if(len(vDir)==3): #3D load.
          for e in xcSet.getElements:
              xElem= e.getCooCentroid(False)[iXCoo]
              if (sigma_v!=0.0) and (xElem>xMin) and (xElem<xMax):
                  e.vector3dUniformLoadGlobal(sigma_v*vDir)
        else: #2D load.
          for e in xcSet.getElements:
              xElem= e.getCooCentroid(False)[iXCoo]
              if (sigma_v!=0.0) and (xElem>xMin) and (xElem<xMax):
                  e.vector2dUniformLoadGlobal(sigma_v*vDir)


class LineVerticalLoadOnBackfill(PressureModelBase):
    '''Lateral earth pressure on a retaining wall due to line surcharge 
    load acting in vertical direction on the backfill. (J.Calavera, pg.41)

    :ivar qLoad: surcharge load (force per unit length)
    :ivar zLoad: global Z coordinate where the line load acts
    :ivar distWall: horizontal distance between the wall and the line 
                    surcharge load
    '''
    def __init__(self,qLoad, zLoad,distWall):
        super(LineVerticalLoadOnBackfill,self).__init__()
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
        return ret_press

class HorizontalLoadOnBackfill(PressureModelBase):
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
        super(HorizontalLoadOnBackfill,self).__init__()
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
        return ret_press

    def appendLoadToCurrentLoadPattern(self,xcSet,vDir,iCoo= 2, delta= 0.0):
        '''Append to the current load pattern the earth thrust on a set of 
        elements due to the horizontal load.

        :param xcSet: set that contains the elements (shells and/or beams)
        :param vDir: unit xc vector defining pressures direction
        '''
        self.setup()
        super(HorizontalLoadOnBackfill,self).appendLoadToCurrentLoadPattern(xcSet,vDir,iCoo,delta)

                                                  
                                                  
