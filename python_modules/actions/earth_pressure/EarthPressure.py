# -*- coding: utf-8 -*-

'''earth_pressure.py: earth pressure over finite element models.'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" 


class EarthPressure:
  def __init__(self,K , zTerrain, gammaTerrain, zWater, gammaWater, vDir):
    self.K= K
    self.zTerrain= zTerrain
    self.gammaTerrain= gammaTerrain
    self.zWater= zWater
    self.gammaWater= gammaWater
    self.vDir= vDir
 
  def getPressure(self,z):
    retval= 0.0
    if(z<self.zTerrain):
      if(z>self.zWater):
        retval=self.K*self.gammaTerrain*(self.zTerrain-z)
      else:
        retval=self.K*self.gammaTerrain*(self.zTerrain-self.zWater)+self.K*(self.gammaTerrain-self.gammaWater)*(self.zWater-z)+self.gammaWater*(self.zWater-z)
    #print K, self.zTerrain, self.zWater, self.gammaTerrain, self.gammaWater,z, retval
    return retval
