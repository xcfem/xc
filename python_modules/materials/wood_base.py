# -*- coding: utf-8 -*-
''' Basic code for wood construction materials.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2021, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

class Wood(object):
    '''Base class for wood materials.
    
    :ivar name: wood name.
    :ivar specificGravity: specific gravity of the wood material.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.
        '''
        self.name= name
        if(specificGravity):
            self.specificGravity= specificGravity
        else:
            self.specificGravity= None
