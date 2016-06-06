# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

#Rough calculation of a tied arch bridge, based on the paper of Maria Garlock
#file: L8-_Tied_Arches_Jan_10.pdf

import math
import ArchBridgeRoughModelBase as base

class TiedArchBridgeRoughModel(base.ArchBridgeRoughModelBase):
  ''' Tied Arch bridge simple model
  
  :ivar d: rise (depth) of the arch at midspan
  :ivar L: horizontal distance between supports
  '''
  def __init__(self,L,d):
    super(TiedArchBridgeRoughModel,self).__init__(L,d)


  def getTensionDeck(self,qunif):
    '''Tension in the deck tie due to a uniform load.
    
    :param qunif: uniformly distributed load applied on the deck
    '''
    H=self.getQunfHabtm(qunif)
    return H
  def getAxialForceArch(self,qunif):
    '''Compressive axial force in the arch due to a uniform load.

     :param qunif: uniformly distributed load applied on the deck
    '''
    V= self.getQunfVabtm(qunif)
    H=self.getQunfHabtm(qunif)
    return math.sqrt(V**2+H**2)

  #Compressive stress
  def getQunfCompStress(self,qunif,A):
    '''Approximation of the compressive stress in a section of the 
    arch with area=A, due to a uniform load qunif.
    ''' 
    N=self.getAxialForceArch(qunif)
    return -N/A  




