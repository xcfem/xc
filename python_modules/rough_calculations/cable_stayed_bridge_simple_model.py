# -*- coding: utf-8 -*-
''' From the notes of María E. Moreyra Garlock in the context of
    the course 'The Art of Structural Engineering: Bridges'''

from __future__ import division

import math

class CableStayedBridgeRoughModel:
  ''' Cable stayed bridge simple model'''
  def __init__(self,l1):
    self.l1= l1 # Tributary length for the cables.

  def getNCable(self,q,theta):
    '''Axial force in a cable
       theta: angle of the cable with the deck.'''
    return q*self.l1/math.sin(theta)

  def getVCable(self,q):
    '''Vertical reaction at tower due to the cable.'''
    return q*l1

  def getHCable(self,q,theta):
    '''Horizontal reaction at tower due to the cable.'''
    return q*self.l1/math.tan(theta)
  
