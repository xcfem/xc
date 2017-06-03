# -*- coding: utf-8 -*-
''' Concrete material according to ACI 318 VERIFICATION TESTS REQUIRED!!!.'''

from materials import concreteBase
from miscUtils import LogMessages as lmsg

__author__= "Ana Ortega (A_OO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, A_OO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com l.pereztato@gmail.com"

class ACIConcrete(concreteBase.Concrete):
    """ Concrete model according to ACI 318

    """
    def __init__(self,nmbConcrete, fck, gammaC):
        super(ACIConcrete,self).__init__(nmbConcrete,fck, gammaC)
        lmsg.error('Implementation of ACI concrete pending\n.')
        
# EHE concretes
A36M= ACIConcrete(nmbConcrete="A36M",fck=-20e6,gammaC=1.5)

