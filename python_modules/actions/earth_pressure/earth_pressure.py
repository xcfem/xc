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
import bisect
import xc_base
import geom
import xc
from miscUtils import LogMessages as lmsg
from geotechnics import mononobe_okabe
from model.sets import sets_mng as sets

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
          tanVector= xc.Vector([vDir[2],vDir[1],-vDir[0]])
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

        

class EarthPressureBase(PressureModelBase):
    '''Parameters to define a load of type earth pressure

      :ivar zGround:   global Z coordinate of ground level
      :ivar gammaSoil: weight density of soil 
    '''
    def __init__(self, zGround, gammaSoil):
        super(EarthPressureBase,self).__init__()
        self.zGround= zGround
        self.gammaSoil= gammaSoil


class EarthPressureModel(PressureModelBase):
    '''Parameters to define a load of type earth pressure

      :ivar zGround: global Z coordinate of ground level
      :ivar zBottomSoils: list of global Z coordinates of the bottom level
            for each soil (from top to bottom)
      :ivar KSoils: list of pressure coefficients for each soil (from top 
            to bottom)
      :ivar gammaSoils: list of weight density for each soil (from top to
            bottom)
      :ivar zWater: global Z coordinate of groundwater level 
            (if zGroundwater<minimum z of model => there is no groundwater)
      :ivar gammaWater: weight density of water
    '''
    def __init__(self, zGround, zBottomSoils,KSoils,gammaSoils, zWater, gammaWater):
        super(EarthPressureModel,self).__init__()
        self.zGround=zGround
        self.zBottomSoils=zBottomSoils
        self.KSoils=KSoils
        self.gammaSoils=gammaSoils
        self.zWater= zWater
        self.zTopLev=[zGround]+zBottomSoils
        self.zTopLev.reverse()
        bisect.insort_left(self.zTopLev,zWater)
        self.zTopLev.reverse()
        indWat=self.zTopLev.index(zWater)
        self.KSoils.insert(indWat-1,self.KSoils[indWat-1])
        self.gammaSoils.insert(indWat-1,self.gammaSoils[indWat-1])
        for i in range(indWat,len(self.gammaSoils)):
            self.gammaSoils[i]-=gammaWater
        self.gammaWater=[0]*indWat+[gammaWater]*(len(self.gammaSoils)-indWat)
            

    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        ret_press=0
        if z <= self.zGround:
            self.zTopLev.reverse()
            ind=len(self.zTopLev)-bisect.bisect_left(self.zTopLev,z)-1
            self.zTopLev.reverse()
            ret_press= 0.0
            for i in range(ind):
                ret_press+=self.KSoils[i]*self.gammaSoils[i]*(self.zTopLev[i]-self.zTopLev[i+1])+self.gammaWater[i]*(self.zTopLev[i]-self.zTopLev[i+1])
            ret_press+=self.KSoils[ind]*self.gammaSoils[ind]*(self.zTopLev[ind]-z)+self.gammaWater[ind]*(self.zTopLev[ind]-z)
        return ret_press

class PeckPressureEnvelope(EarthPressureBase):
    ''' Envelope of apparent lateral pressure diagrams for design 
        of cuts in sand. See 10.2 in the book "Principles of Foundation
        Engineering" from Braja M. Das.

      :ivar H:         height of the cut.
      :ivar phi:       effective friction angle of sand.
    '''
    def __init__(self,phi , zGround, gammaSoil, zWater, gammaWater, H):
        self.K= math.tan(math.pi/4.0-phi/2.0)**2 # Rankine active pressure coefficient.
        self.zWater=zWater
        self.gammaWater=gammaWater
        super(PeckPressureEnvelope,self).__init__( zGround, gammaSoil)
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

class UniformLoadOnBackfill(UniformLoadOnStem):
    '''Lateral earth pressure on a retaining wall due to a uniform indefinite
       load on the backfill.

    :ivar qLoad: surcharge load (force per unit area)
    '''
    def __init__(self,K, qLoad):
        super(UniformLoadOnBackfill,self).__init__(qLoad)
        self.K= K
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        return self.K*self.qLoad

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
            l= e.getLineSegment(False).getLength()
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

    def getMaxMagnitude(self,xcSet):
        '''Return an estimation of the maximum magnitude of the vector loads 
        (it's supposed to occur in a point placed 1/3L from the top)'''
        zmin=sets.get_min_coo_nod(xcSet,2)
        zmax=sets.get_max_coo_nod(xcSet,2)
        zcontrol=zmin+2/3.*(zmax-zmin)
        maxEstValue=self.getPressure(zcontrol)
        return maxEstValue


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

    def getMaxMagnitude(self,xcSet):
        '''Return an estimation of the maximum magnitude of the vector loads 
        (it's supposed to occur in a point placed 1/3L from the top)'''
        zmin=sets.get_min_coo_nod(xcSet,2)
        zmax=sets.get_max_coo_nod(xcSet,2)
        zcontrol=zmin+2/3.*(zmax-zmin)
        maxEstValue=self.getPressure(zcontrol)
        return maxEstValue

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

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        self.setup()
        maxValue=self.getPressure(self.zpresmax)
        return maxValue
                                                  
                                                  
class MononobeOkabePressureDistribution(EarthPressureBase):
    '''Overpressure due to seismic action according to Mononobe-Okabe

      :ivar H: height of the structure.
      :ivar kv: seismic coefficient of vertical acceleration.
      :ivar kh: seismic coefficient of horizontal acceleration.
      :ivar psi: back face inclination of the structure (< PI/2)
      :ivar phi: angle of internal friction of soil.
      :ivar delta_ad: angle of friction soil - structure.
      :ivar beta: slope inclination of backfill.
      :ivar Kas: static earth pressure coefficient 
    '''
    def __init__(self,zGround, gamma_soil, H, kv, kh, psi, phi, delta_ad, beta, Kas):
        super(MononobeOkabePressureDistribution,self).__init__(zGround, gamma_soil)
        self.H= H
        self.kv= kv
        self.kh= kh
        self.psi= psi
        self.phi= phi
        self.delta_ad= delta_ad
        self.beta= beta
        self.Kas= Kas
        self.update()
        
    def update(self):
        self.overpressure_dry= mononobe_okabe.overpressure_dry(self.H, self.gammaSoil, self.kv, self.kh, self.psi, self.phi, self.delta_ad, self.beta, self.Kas)
        self.max_stress= 2*self.overpressure_dry/self.H
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z
        '''
        zSup= self.zGround
        zInf= self.zGround-self.H
        retval= 0.0
        if((z>=zInf) and (z<=zSup)):
            retval= (z-zInf)/(zSup-zInf)*self.max_stress
        return retval

