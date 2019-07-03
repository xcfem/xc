# -*- coding: utf-8 -*-
from __future__ import division
import math

# Krey's lateral earth pressure under concentrated load. 
#
#    Pressure distribution:
#   
#           a
#     +-----------+     <----- p_max # Maximum pressure.
#     |           |     <----
#   b |           |     <---
#     |           |     <--
#     +-----------+     <      0.0   # Zero pressure.
#
#     ^^^^^^^^^^^^^
#      |||||||||||
#        |||||||
#          |||
#           |           p_max
#
#      Coordinate system.
#     
#      ^ y
#      |
#      |
#      |         P
#      |        +       ------> x
#      |
#      |
#      |
#           a
#      +--------+

class KreyEarthPressurUnderConcentratedLoad(object):
  P= None # Punctual load.
  a= None # Loaded surface width == Distance from load to wall.
  b= None # Loaded surface height.
  p_max= None # Maximum pressure 
  Q= None # Integral of pressures.
  fi= None # Friction angle of soil.
  def __init__(self,P,a,fi):
    self.P= P
    self.a= a
    self.fi= fi
    tg1= math.tan(math.pi/4.0+self.fi/2.0)
    self.Q= P * tg1
    self.b= a*(tg1-math.tan(fi))
    self.p_max= 4*self.Q/self.a/self.b

  def getZLimSup(self):
    return -self.a*math.tan(self.fi)
  def getZLimInf(self):
    return self.getZLimSup()-self.b

  def getRatioY(self,y,z):
    ratioY= 0.0
    y_abs= abs(y)
    if(y_abs<self.a/2.0):
      ratioY= 1-2*y_abs/self.a
    return ratioY

  def getRatioZ(self,y,z):
    z_lim_sup= self.getZLimSup()
    z_lim_inf= self.getZLimInf()
    ratioZ= 0.0
    if((z<=z_lim_sup) and (z>=z_lim_inf)):
      ratioZ= (z-z_lim_inf)/self.b
    return ratioZ

  def getLateralPressure(self,y,z):
    # Returns pressure at position (x,y,z).
    return self.p_max*self.getRatioY(y,z)*self.getRatioZ(y,z)
