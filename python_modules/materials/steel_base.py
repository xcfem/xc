# -*- coding: utf-8 -*-
from __future__ import division
''' Base classes and function for structural steel.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from materials import typical_materials
from misc_utils import log_messages as lmsg

class BasicSteel(typical_materials.BasicElasticMaterial):
    '''Base class for structural steels.

      :ivar fy:  yield stress.
      :ivar fu: ultimate stress.
      :ivar gammaM: material partial safety factor.
    '''
    
    alpha= 1.2e-5 # coefficient of thermal expansion (1/ºC)
    
    def __init__(self, E, nu, fy, fu, gammaM):
        super(BasicSteel,self).__init__(E,nu,rho=7850)
        self.fy= fy
        self.fu= fu
        self.gammaM= gammaM

    def fyd(self):
        return self.fy/self.gammaM

    def getDict(self):
        ''' Returns a dictionary whith the values of the internal forces.
            Makes easier export it to json.'''
        retval= super(BasicSteel,self).getDict()
        retval.update({'fy':self.fy,'fu':self.fu,'gammaM':self.gammaM})
        return retval

    def setFromDict(self,dct):
        '''Sets the internal forces from the dictionary argument.'''
        super(BasicSteel,self).setFromDict(dct)
        self.fy= dct['fy']
        self.fu= dct['fu']
        self.gammaM= dct['gammaM']
