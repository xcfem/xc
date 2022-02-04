# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to the
    National Design Specification of the American Wood Council.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg
from materials import wood_member_base
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd

class Member(wood_member_base.Member):
    ''' Beam and column members according to ANSI AISC 360-16.

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, Cb= None, lstLines=None):
        ''' Constructor. 

        :param name: object name.
        :param crossSection: timber cross-section.
        :param lstLines: ordered list of lines that make up the beam.
        '''
        super(Member,self).__init__(name, section, lstLines)
        self.unbracedLengthX= unbracedLengthX
        if(unbracedLengthY):
            self.unbracedLengthY= unbracedLengthY
        else:
            self.unbracedLengthY= unbracedLengthX
        if(unbracedLengthZ):
            self.unbracedLengthZ= unbracedLengthZ
        else:
            self.unbracedLengthZ= unbracedLengthX            
