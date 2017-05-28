# -*- coding: utf-8 -*-

'''earth_pressure.py: earth pressure on finite element models.'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
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
    '''Return the earth pressure acting on the points at global coordinate z 
    '''
    retval= 0.0
    if(z<self.zGround):
      if(z>self.zWater):
        retval=self.K*self.gammaSoil*(self.zGround-z)
      else:
        retval=self.K*self.gammaSoil*(self.zGround-self.zWater)+self.K*(self.gammaSoil-self.gammaWater)*(self.zWater-z)+self.gammaWater*(self.zWater-z)
    return retval
