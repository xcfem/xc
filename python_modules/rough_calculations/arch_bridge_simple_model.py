# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2017, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import math
from rough_calculations import arch_bridge_rough_base as base

class ArchBridgeRoughModel(base.ArchBridgeRoughModelBase):
  ''' Arch bridge simple model'''
  def __init__(self,L,d):
    super(ArchBridgeRoughModel,self).__init__(L,d)

  #Compressive stress
  def getQunfCompStress(self,qunif,A):
    '''Approximation of the compressive stress in a section of the 
    arch with area=A, due to a uniform load qunif.
    for simplicity, it's considered the compressive force at the midspan
    (H, that, in turn, i equal to the horizontal reaction at the abutment)
    as an approximation for the compressive force in the arch at any location
    '''  
    return -self.getQunfHabtm(qunif)/A  

  #*Concentrated loads in three hinged arch at each quarterpoint 
  #Abutment reactions
  def getQconcVabtm(self,Qconc):
    '''Vertical reaction at each abutment of a three hinged arch 
    due to two concentrated loads in quarterpoints
    '''
    return Qconc

  def getQconcHabtm(self,Qconc):
    '''Horizontal reaction at each abutment of a three hinged arch 
    due to a two concentrated loads in quarterpoints
    '''
    return Qconc*self.L/4.0/self.d
  #Compressive stress
  def getQconcCompStress(self,Qconc,A):
    '''Approximation of the compressive stress in a section of the 
    three hinged arch with area=A, due to a two concentrated loads in quarterpoints.
    for simplicity, it's considered the compressive force at the midspan
    (H, that, in turn, i equal to the horizontal reaction at the abutment)
    as an approximation for the compressive force in the arch at any location
    '''  
    return -self.getQconcHabtm(Qconc)/A  
  #Bending moments in a parabolic three hinged arch produced by 
  #concentrated live forces placed at quarterspans
  def getMmaxQconc(self,Qconc):
    '''maximum bending moment in a parabolic three hinged arch
    produced by concentrated live forces placed at quarterspans
    The bending moment is maximum at the quarterpoints
    '''
    MV=Qconc*self.L/4  #due to vertical reactions at abutments
    MH=-3*self.getQconcHabtm(Qconc)*self.d/4 #due to horizontal reactions at abutments
    return MV+MH

