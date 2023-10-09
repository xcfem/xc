# -*- coding: utf-8 -*-
'''Rail loads due to cant.'''

from __future__ import print_function
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2023, A_OO   LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"


import math
import geom

class TrackCrossSection(object):
    ''' Track cross section geometry. 

    :ivar s: distance between rail axes.
    :ivar u: difference of external and internal rail heads height (cant).
    '''
    def __init__(self, s, u, org= geom.Pos2d(0,0)):
        ''' Constructor.

        :param s: distance between rail axes.
        :param u: difference of external and internal rail heads height (cant).
        '''
        self.s= s
        self.u= u
        self.org= org

    def getRailPositions(self, h):
        ''' Return the positions of the rails.

        :param h: distance from the center of gravity of the train to the 
                  reference plane.
        '''
        # Compute the coordinates of the rails.
        theta= math.atan2(self.u, self.s)
        ct= math.cos(theta)
        st= math.sin(theta)
        downVector= h*geom.Vector2d(st, -ct)
        halfS= self.s/2.0
        leftVector= halfS*geom.Vector2d(-ct,-st)
        leftRailPos= self.org+downVector+leftVector
        rightRailPos= self.org+downVector-leftVector
        return leftRailPos, rightRailPos

    def getRailLoads(self, Q, h):
        ''' Loads on each rail considring the cant. 

        :param Q: load vector [Fh, Fv, M] in the center of gravity of the train.
        :param h: distance from the center of gravity of the train to the 
                  reference plane.
        '''
        # Create sliding vectors system.
        org= geom.Pos2d(0,0)
        force= geom.Vector2d(Q[0], Q[1])
        moment= Q[2]
        loadSVS= geom.SlidingVectorsSystem2d(self.org, force, moment)
        # Compute the coordinates of the rails.
        leftRailPos, rightRailPos= self.getRailPositions(h= h)
        # Compute the forces.
        [f0, f1]= loadSVS.distribute([leftRailPos, rightRailPos])
        return [f0.x, f0.y, f1.x, f1.y]
        

def get_rail_loads(Q, h, s, u):
    ''' Loads on each rail due to cant. 

    :param Q: load vector [Fh, Fv, M] in the center of gravity of the train.
    :param h: distance from the center of gravity of the train to the reference
              plane.
    :param s: distance between rail axes.
    :param u: difference of external and internal rail heads height (cant).
    '''
    # Create sliding vectors system.
    cs= TrackCrossSection(s= s, u= u)
    return cs.getRailLoads(Q,h)
