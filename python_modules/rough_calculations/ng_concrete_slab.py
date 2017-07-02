# -*- coding: utf-8 -*-
from __future__ import division
import scipy.interpolate
import math

class ConcreteSlab(object):
  '''Concrete slab preliminary structural calculations'''
  def __init__(self,l,L,thickness):
    ''' Constructor

    :param l: short side of the slab.
    :param L: long side of the slab.
    :param thickness: thickness of the slab.
    '''
    self.L= max(L,l)
    self.l= min(L,l)
    self.thickness= thickness

class FourSidesPinnedConcreteSlab(ConcreteSlab):
  '''Four sides pinned concrete slab preliminary structural calculations'''
  def __init__(self,l,L,thickness):
    ''' Constructor '''
    super(FourSidesPinnedConcreteSlab,self).__init__(l,L,thickness)

  def getMMax(self,p):
    ''' Returns maximum bending moment under the load p

    :param p: Total load over the slab.

    .. note:: Formulas from Yvon Lescouarc'h ingénieur.
    '''
    a= 8.0
    if(self.l/self.L>=0.4):
      x= [0.4,0.5,0.6,0.7,0.8,0.9,1] #l/L
      y= [8.9,10.2,11.6,13.5,15.9,18.9,22.7]
      fn= scipy.interpolate.interp1d(x,y)
      a= fn(self.l/self.L)
    return p*self.l*self.l/a

  def predimThickness(self,p):
    ''' Returns thickness for a four side supported slab

    :param p: Total load over the slab (N/m2).

    .. note:: Formulas from Yvon Lescouarc'h ingénieur.
    '''
    return math.sqrt(self.getMMax(p)/100.0)/100.0
