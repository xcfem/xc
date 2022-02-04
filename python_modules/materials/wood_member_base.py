# -*- coding: utf-8 -*-
''' Base classes and functions for structural timber members.'''

from __future__ import division


__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg
from materials import member_base

class Member(member_base.Member):
    '''Base class for steel members.
    
    :ivar crossSection: timber cross-section.
    '''
    def __init__(self,name, crossSection, lstLines=None):
        '''Constructor.

        :param name: object name.
        :param shape: cross-section steel shape.
        :param lstLines: ordered list of lines that make up the member 
                        (defaults to None).
        '''
        super(Member,self).__init__(name, lstLines)
        self.crossSection= crossSection
