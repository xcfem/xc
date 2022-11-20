# -*- coding: utf-8 -*-
''' From the notes of María E. Moreyra Garlock in the context of
    the course 'The Art of Structural Engineering: Bridges'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import math

class CableStayedBridgeRoughModel:
    ''' Cable stayed bridge simple model'''
    
    def __init__(self, l1):
        self.l1= l1 # Tributary length for the cables.

    def getNCable(self,q,theta):
        '''Axial force in a cable

        :param q: uniform load on the bridge deck.
        :param theta: angle of the cable with the deck.
        '''
        return q*self.l1/math.sin(theta)

    def getVCable(self,q):
        '''Vertical reaction at tower due to the cable.

        :param q: uniform load on the bridge deck.
        '''
        return q*self.l1

    def getHCable(self, q, theta):
        '''Horizontal reaction at tower due to the cable.

        :param q: uniform load on the bridge deck.
        :param theta: angle of the cable with the deck.
        '''
        return q*self.l1/math.tan(theta)
  
