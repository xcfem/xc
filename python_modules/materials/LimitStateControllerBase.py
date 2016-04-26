# -*- coding: utf-8 -*-
#Basic parameters for limit state control.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg

class LimitStateControllerBase(object):
  '''
  Basic parameters for limit state control (normal stresses, shear, crack,...).'''
  def __init__(self,limitStateLabel):
    self.limitStateLabel= limitStateLabel #Property name in the check results file (something like 'ULS_shear' or 'SLS_crack_freq' or ...) 

  def check(self,elements,nmbComb):
    '''Crack control.'''
    lmsg.error('limit state check not implemented.')

