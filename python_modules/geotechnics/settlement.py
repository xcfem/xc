# -*- coding: utf-8 -*-

from __future__ import division
import math

'''Soil settlement.

   Code based on the chapter 5 of the book "Principles of 
   Foundation Engineering, SI Edition.
   Seventh edition. Braja M. Das.
   ISBN-13: 978-0-495-66812-1
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2017, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def stress_increment_under_concentrated_load(P, x, y, z):
  ''' Return the increment in the vertical stress of a point inside an
      homogeneous and elastic soil due to a load on the surface.

      :param P: value of the load.
      :param x: x-coordinate of the point relative to the loaded point.
      :param y: y-coordinate of the point relative to the loaded point.
      :param z: z-coordinate of the point relative to the loaded point.
  '''
  r= math.sqrt(x**2+y**2)
  denom= 2.0*math.pi*z**2*math.pow(1+(r/z)**2,5.0/2.0)
  return 3.0*P/denom

def average_stress_increment_under_concentrated_load(P, x, y, z1, z2, zInc= 0.2):
  ''' Return the average increment in the vertical stress of a the points
      between z1 and z2.

      :param P: value of the load.
      :param x: x-coordinate of the point relative to the loaded point.
      :param y: y-coordinate of the point relative to the loaded point.
      :param z1: upper limit of the z-coordinate.
      :param z2: lower limit of the z-coordinate.
      :param zInc: size of the discretization.
  '''
  H= z2-z1
  nDivZ= int(math.ceil(H/zInc))
  retval= 0.0
  z= z1+zInc/2.0
  for i in range(0,nDivZ):
    retval+= stress_increment_under_concentrated_load(P, x, y, z)
    z+= zInc
  retval/= nDivZ
  return retval

def stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z, eSize= 0.1):
  ''' Return the increment in the vertical stress of a point inside an
      homogeneous and elastic soil due to a load distributed on
      a rectangular area.

      :param q: pressure on the loaded area.
      :param B: dimension of the rectangular area in direction x.
      :param L: dimension of the rectangular area in direction y.
      :param eSize: length of the side for the discretization.
      :param x: x-coordinate of the point relative to the rectangle center.
      :param y: y-coordinate of the point relative to the rectangle center.
      :param z: z-coordinate of the point relative to the rectangle center.
  '''
  nDivX= int(math.ceil(B/eSize))
  nDivY= int(math.ceil(L/eSize))
  xLoad= -B/2.0+eSize/2.0 
  yLoad= -L/2.0+eSize/2.0
  P= q*eSize**2 # punctual load.
  retval= 0
  for i in range(0,nDivX):
    for j in range(0,nDivY):
      retval+= stress_increment_under_concentrated_load(P, x-xLoad, y-yLoad, z)
      yLoad+= eSize
    xLoad+= eSize
    yLoad= -L/2.0+eSize/2.0 # from square one.
  return retval

def average_stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z1, z2, eSize= 0.1, zInc= 0.2):
  ''' Return the average increment in the vertical stress of the
      points between z1 and z2 inside an homogeneous and elastic soil 
      due to a load distributed on a rectangular area.

      :param q: pressure on the loaded area.
      :param B: dimension of the rectangular area in direction x.
      :param L: dimension of the rectangular area in direction y.
      :param x: x-coordinate of the point relative to the rectangle center.
      :param y: y-coordinate of the point relative to the rectangle center.
      :param z: z-coordinate of the point relative to the rectangle center.
      :param z1: upper limit of the z-coordinate.
      :param z2: lower limit of the z-coordinate.
      :param zInc: size of the discretization.
  '''
  H= z2-z1
  nDivZ= int(math.ceil(H/zInc))
  retval= 0.0
  z= z1+zInc/2.0
  for i in range(0,nDivZ):
    retval+= stress_increment_under_rectangular_loaded_area(q, B, L, x, y, z, eSize= 0.01)
    z+= zInc
  retval/= nDivZ
  return retval
