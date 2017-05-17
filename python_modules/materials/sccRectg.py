# -*- coding: utf-8 -*-
from __future__ import division
# Sección rectangular

import math
import xc_base
import geom


'''

   b: Ancho de la sección.
   h: Canto de la sección.
  
'''
class sccRectang(object):
  """Rectangular section."""
  def area(self):
    """Devuelve el área de la sección,"""
    return self.b*self.h
  def I1(self):
    """Devuelve el momento de inercia respecto al eje paralelo al width que pasa por el CDG de la sección."""
    return 1.0/12.0*self.b*self.h**3
  def i1(self):
    """Devuelve el radius de giro respecto al eje paralelo al width que pasa por el CDG de la sección."""
    return math.sqrt(self.I1()/self.area())
  def Me1(self,fy):
    """Devuelve el momento elástico respecto al eje paralelo al width que pasa por el CDG de la sección. """
    return 2*fy/self.h*self.I1()
  def S1PosG(self):
    """ Momento estático de la mitad de la sección respecto al eje paralelo al width por el CDG."""
    return (self.b*self.h/2.0)*self.h/4.0
  def Mp1(self,fy):
    """ Devuelve el momento plástico respecto al eje paralelo al width que pasa por el CDG de la sección."""
    return 2*self.S1PosG()*fy

  def I2(self):
    """Devuelve el momento de inercia respecto al eje paralelo al depth que pasa por el CDG de la sección."""
    return 1.0/12.0*self.h*self.b**3

  def i2(self):
    """Devuelve el radius de giro respecto al eje paralelo al depth que pasa por el CDG de la sección."""
    return math.sqrt(self.I2()/self.area())

  def Me2(self,fy):
    """Devuelve el momento elástico respecto al eje paralelo al depth que pasa por el CDG de la sección."""
    return 2*fy/self.b*self.I2()
  def S2PosG(self):
    """Momento estático de la mitad de la sección respecto al eje paralelo al width por el CDG."""
    return (self.b*self.h/2.0)*self.b/4.0
  def Mp2(self,fy):
    """Devuelve el momento plástico respecto al eje paralelo al depth que pasa por el CDG de la sección."""
    return 2*self.S2PosG()*fy

  def discretization(self,gm,nmbMat):
    '''generation of a quadrilateral region from the rectangular section 
    geometry (sizes and number of divisions for the cells)
    made of the specified material
   
    :param gm: object of type section_geometry
    :param nmbMat: name of the material (string)
    '''
    regiones= gm.getRegions
    reg= regiones.newQuadRegion(nmbMat)
    reg.nDivIJ= self.nDivIJ
    reg.nDivJK= self.nDivJK
    reg.pMin= geom.Pos2d(-self.h/2.0,-self.b/2.0)
    reg.pMax= geom.Pos2d(self.h/2.0,self.b/2.0)
    return reg

