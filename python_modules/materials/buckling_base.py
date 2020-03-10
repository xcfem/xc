# -*- coding: utf-8 -*-
from __future__ import division
''' Base classes and function for structural steel.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

class MemberConnection(object):
    '''Member connection

       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        self.rotI= rotI
        self.transI= transI
        self.rotJ= rotJ
        self.transJ= transJ
    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the effective length factor
           according to table C-A-7.1 or AISC specification
           and NDS 2018 appendix G'''
        if(self.rotI=='fixed'):
            if(self.rotJ=='fixed'):
                if(self.transJ=='fixed'):
                    retval= .65 # Theoretical .5
                else: # self.transJ=='free'
                    retval= 1.2 #Theoretical 1.0
            else: # self.rotJ=='free'
                if(self.transJ== 'fixed'):
                    retval= .8 # Theoretical .7
                else: # self.transJ=='free'
                    retval= 2.1 # Theoretical 2.0
        else: # self.rotI=='free'
             if(self.rotJ=='fixed'):
                 if(self.transJ=='free'):
                     retval= 2.0 # Theoretical 2.0
                 else:
                     retval= 0.8 # Theoretical .7
             else: # self.rotJ=='free'
                 if(self.transI=='fixed' and self.transJ=='fixed'):
                     retval= 1.0 # Theoretical 1.0
                 else:
                     retval= 1e6 # Stiffness matrix singular
        return retval


