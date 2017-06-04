# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to ACI 318. VERIFICATION TESTS REQUIRED!!!.'''

from materials import concreteBase
from miscUtils import LogMessages as lmsg
from materials import reinforcingSteel

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
        
# ACI concretes
A36M= ACIConcrete(nmbConcrete="A36M",fck=-20e6,gammaC=1.5)

# Reinforcing steel.

A706G60= reinforcingSteel.ReinforcingSteel(nmbAcero="A706G60", fyk=420e6, emax=0.08,gammaS=1.15)


#Bar areas in square meters.

num3Area= 71e-6
num4Area= 129e-6
num5Area= 200e-6
num6Area= 284e-6
num7Area= 387e-6
num8Area= 509e-6
num9Area= 645e-6
num10Area= 819e-6
num11Area= 1006e-6
num14Area= 1452e-6
num18Area= 2581e-6
num18JArea= 2678e-6
