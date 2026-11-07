# -*- coding: utf-8 -*-
''' Base class for soil layers representation.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
from scipy.constants import g
from misc_utils import log_messages as lmsg

class SoilLayers(object):
    ''' Base class for soil horizons in pile computations.

    :ivar waterTableZ: global Z coordinate of the water table. 
    :ivar groundLevel: global Z coordinate of the ground level.
    :ivar waterDensity: density of the water.
    :ivar clay_Cd: drag resistance coefficient for clays (defaults to 0.3).
    :ivar sand_Cd: drag resistance coefficient for clays (defaults to 1.0).
    :ivar suction: Uplift resistance is equal to suction*qult. Default = 0.0. 
                   The value of suction must be 0.0 to 0.1.
    '''
    def __init__(self, waterTableZ= -math.inf, groundLevel= 0.0, waterDensity= 1e3, clay_Cd= 0.3, sand_Cd= 0.3, suction= 0.0):
        ''' Constructor.

        :param waterTableZ: global Z coordinate of the water table.
        :param groundLevel: global Z coordinate of the ground level.
        :param waterDensity: density of the water.
        :param clay_Cd: drag resistance coefficient for clays (defaults to 0.3).
        :param sand_Cd: drag resistance coefficient for clays (defaults to 1.0).
        :param suction: Uplift resistance is equal to suction*qult. 
                        Default = 0.0. The value of suction must be 0.0 to 0.1.
        '''
        self.waterTableZ= waterTableZ
        self.groundLevel= groundLevel
        self.waterDensity= waterDensity
        self.clay_Cd= clay_Cd
        self.sand_Cd= sand_Cd
        self.suction= max(min(suction, 0.1), 0.0)

    def submerged(self, z):
        ''' Return true if z<waterTableZ.'''
        return z<self.waterTableZ

    def getWaterTableDepth(self):
        ''' Return the depth of the water table.'''
        if(self.waterTableZ is not None):
            retval= self.groundLevel-self.waterTableZ
        else:
            retval= math.inf
        return retval

    def getWaterPressure(self, depth):
        ''' Return the pressure of the water (if any) at the given depth.

        :param depth: depth of the desired soil.
        '''
        retval= 0.0
        waterTableDepth= self.getWaterTableDepth()
        if(depth>waterTableDepth):
            waterThickness= depth-waterTableDepth
            retval= waterThickness*self.waterDensity*g
        return retval

    def getPorePressureAtDepth(self, depth):
        ''' Return the pore pressure at the given depth.

        :param depth: depth of the poing of interest (m).
        '''
        return self.getWaterPressure(depth)

    def getSuctionCoefficient(self):
        ''' Return the suction coefficient. Uplift resistance is equal to 
            suction*qult. Default = 0.0. The value of suction must be 0.0 
            to 0.1.
        '''
        return self.suction
