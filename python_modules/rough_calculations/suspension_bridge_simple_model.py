# -*- coding: utf-8 -*-

from __future__ import division

import math

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

  def getHb(self,q):
    '''Horizontal reaction at tower due to the back span.'''
    return self.getHm(q)

  def getVanchor(self,q):
    '''Vertical reaction at anchor.'''
    return self.getVb(q)-q*self.Lb

  
