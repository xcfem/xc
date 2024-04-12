# -*- coding: utf-8 -*-
'''Material stresses.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

class Stresses2D(object):
    '''2D solid mechanics stresses.

    :ivar sigma_11: (1,1) component of the stress tensor.
    :ivar sigma_12: (1,2) component of the stress tensor.
    :ivar sigma_22: (2,2) component of the stress tensor.
    '''
    def __init__(self, sigma_11= 0.0, sigma_12= 0.0, sigma_22= 0.0):
        ''' Constructor.

        :param sigma_11:  (1,1) component of the stress tensor.
        :param sigma_12:  (1,2) component of the stress tensor.
        :param sigma_22:  (2,2) component of the stress tensor.
        '''
        self.sigma_11= sigma_11 # (1,1) component of the stress tensor.
        self.sigma_12= sigma_12 # (1,2) component of the stress tensor.
        self.sigma_22= sigma_22 # (2,2) component of the stress tensor.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= {'sigma_11':self.sigma_11, 'sigma_12':self.sigma_12, 'sigma_22':self.sigma_22}
        return retval
       
    def setStressesFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.sigma_11= dct['sigma_11']
        self.sigma_12= dct['sigma_12']
        self.sigma_22= dct['sigma_22']

    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.setStressesFromDict(dct= dct)

class Stresses3D(Stresses2D):
    '''3D solid mechanics stresses.

    :ivar sigma_13: (1,3) component of the stress tensor.
    :ivar sigma_23: (2,3) component of the stress tensor.
    :ivar sigma_33: (3,3) component of the stress tensor.
    '''
    def __init__(self, sigma_11= 0.0, sigma_12= 0.0, sigma_13= 0.0, sigma_22= 0.0, sigma_23= 0.0, sigma_33= 0.0):
        ''' Constructor.

        :param sigma_11: (1,1) component of the stress tensor.
        :param sigma_12: (1,2) component of the stress tensor.
        :param sigma_13: (1,3) component of the stress tensor.
        :param sigma_22: (2,2) component of the stress tensor.
        :param sigma_23: (2,3) component of the stress tensor.
        :param sigma_33: (3,3) component of the stress tensor.
        '''
        super().__init__(sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22)
        self.sigma_13= sigma_13 # (1,3) component of the stress tensor.
        self.sigma_23= sigma_23 # (2,3) component of the stress tensor.
        self.sigma_33= sigma_33 # (2,2) component of the stress tensor.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super().getDict()
        retval.update({'sigma_13':self.sigma_13, 'sigma_23':self.sigma_23, 'sigma_33':self.sigma_33})
        return retval
       
    def setStressesFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super().setStressesFromDict(dct= dct)
        if('sigma_13' in dct):
            self.sigma_13= dct['sigma_13']
            self.sigma_23= dct['sigma_23']
            self.sigma_33= dct['sigma_33']

