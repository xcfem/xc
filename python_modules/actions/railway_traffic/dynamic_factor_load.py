# -*- coding: utf-8 -*-
''' Base class for railway loads.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2023,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import sys
import geom
import xc
from misc_utils import log_messages as lmsg

class DynamicFactorLoad(object):
    ''' Base class for railway loads.

    :ivar dynamicFactor: dynamic factor affecting the load.
    :ivar classificationFactor: classification factor (on lines carrying rail
                                traffic which is heavier or lighter than
                                normal rail traffic).
    '''
    def __init__(self, dynamicFactor, classificationFactor= 1.21):
        ''' Constructor.

        :param dynamicFactor: dynamic factor.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        self.dynamicFactor= dynamicFactor
        self.classificationFactor= classificationFactor

    def setDynamicFactor(self, dynamicFactor):
        ''' Set the dynamic factor for the load.

        :param dynamicFactor: dynamic factor.
        '''
        self.dynamicFactor= dynamicFactor

    def setClassificationFactor(self, classificationFactor):
        ''' Set the classification factor for the load.

        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        self.classificationFactor= classificationFactor
