# -*- coding: utf-8 -*-

from __future__ import division

import math

_author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

#Rough calculation of a suspension bridge, based on the paper of Maria Garlock

class SuspensionBridgeRoughModel:
  ''' Suspension bridge simple model'''
  def __init__(self,sag,da,Lm,Lb):
    self.d= sag # sag.
    self.da= da # Vertical distance from the top of the tower to the anchor.
    self.Lm= Lm # Main span length.
    self.Lb= Lb # Back span length.

  def getVm(self,q):
    '''Vertical reaction at tower due to the main span.'''
    return q*self.Lm/2.0

  def getVb(self,q):
    '''Vertical reaction at tower due to the main span.'''
    return ((self.getHb(q)*self.da)+(q*self.Lb**2/2.0))/self.Lb

  def getHm(self,q):
    '''Horizontal reaction at tower due to the main span.'''
    return q*self.Lm**2/(8.0*self.d)

  def getCableSlopeAtTower(self,q):
    '''Returns the slope (angle) of the cable at the tower'''
    return math.atan2(self.getVm(q),self.getHm(q))

  def getCableAxialForceAtTower(self,q):
    '''Returns the axial force of the cable at the tower'''
    return math.sqrt(self.getVm(q)**2+self.getHm(q)**2)

  def getCableAxialForceAtMidSpan(self,q):
    '''Returns the axial force of the cable at the tower'''
    return self.getHm(q)

  def getHb(self,q):
    '''Horizontal reaction at tower due to the back span.'''
    return self.getHm(q)

  def getVanchor(self,q):
    '''Vertical reaction at anchor.'''
    return self.getVb(q)-q*self.Lb

  
