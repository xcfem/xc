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
from misc_utils import log_messages as lmsg

class SoilLayers(object):
    ''' Base class for soil horizons in pile computations.

    :ivar waterTableZ: global Z coordinate of the water table. 
    :ivar groundLevel: global Z coordinate of the ground level.
    :ivar waterDensity: density of the water.
    '''
    def __init__(self, waterTableZ= -math.inf, groundLevel= 0.0, waterDensity= 1e3):
        ''' Constructor.

        :param waterTableZ: global Z coordinate of the water table.
        :param groundLevel: global Z coordinate of the ground level.
        :param waterDensity: density of the water.
        '''
        self.waterTableZ= waterTableZ
        self.groundLevel= groundLevel
        self.waterDensity= waterDensity

    def submerged(self, z):
        ''' Return true if z<waterTableZ.'''
        return z<self.waterTableZ


