# -*- coding: utf-8 -*-
''' Steel as specified American Iron and Steel Institute standards.

'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2021, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import math
from materials import steel_base

class AISISteel(steel_base.BasicSteel):
    '''AISI structural steel.

    :ivar name: steel identifier.
    :ivar fy: yield stress (defaults to 250e6 Pa)
    :ivar fu: ultimate tensile strength (defaults to 400e6 Pa)
    :ivar gammaM: partial factor (defaults to 1.0)
    :ivar Rt: Ratio of the expected tensile strength to the 
              specified minimum tensile strength, Fu.
              See table A3.1 of AISC 341 seismic provisions.
    :ivar Ry: Ratio of the expected yield stress to the specified
              minimum yield stress, Fy.
              See table A3.1 of AISC 341 seismic provisions.
    '''
    def __init__(self, name= None, E= 193e9, fy= 250e6, fu= 400e6, gammaM= 1.0, Rt= None, Ry= None):
        ''' Constructor.

        :param name: steel identifier.
        :param E:  elastic modulus.
        :param fy: yield stress (defaults to 250e6 Pa)
        :param fu: ultimate tensile strength (defaults to 400e6 Pa)
        :param gammaM: partial safety factor (defaults to 1.0)
        '''
        super(AISISteel,self).__init__(E= 200e9, nu=0.3, fy= fy, fu= fu, gammaM= gammaM)
        if(name):
            self.name= name
        else:
            self.name= None

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(AISISteel,self).getDict()
        name= None
        if(self.name):
            name= self.name
        retval.update({'name': name})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= None
        if('name' in dct):
            name= dct['name']
            if(name): self.name= name

# Data obtained from http://www.matweb.com/
AISI_304L= AISISteel(name='AISI-304L', E= 200e9, fy= 216e6, fu= 546e6, gammaM= 1.0)
AISI_316L= AISISteel(name='AISI-316L', E= 193e9, fy= 290e6, fu= 560e6, gammaM= 1.0)



