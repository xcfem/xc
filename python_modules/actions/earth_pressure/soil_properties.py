# -*- coding: utf-8 -*-
from __future__ import division

'''Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) exerced by a given back soil with 
or without water level
'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 


class SoilProp:
    '''Parameters to define a load of type earth pressure

      :ivar K:         coefficient of pressure
      :ivar zGround:   global Z coordinate of ground level
      :ivar gammaSoil: weight density of soil 
      :ivar zWater:    global Z coordinate of groundwater level 
                       (if zGroundwater<minimum z of model => there is no groundwater)
      :ivar gammaWater: weight density of water
    '''
    def __init__(self,K , zGround, gammaSoil, zWater, gammaWater):
        self.K= K
        self.zGround= zGround
        self.gammaSoil= gammaSoil
        self.zWater= zWater
        self.gammaWater= gammaWater

    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z        '''
        ret_press= 0.0
        if(z<self.zGround):
            if(z>self.zWater):
                ret_press=self.K*self.gammaSoil*(self.zGround-z)
            else:
                ret_press=self.K*self.gammaSoil*(self.zGround-self.zWater)+self.K*(self.gammaSoil-self.gammaWater)*(self.zWater-z)+self.gammaWater*(self.zWater-z)
        print 'z,pres', z,',', ret_press
        return ret_press
