# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to ACI 318. 
VERIFICATION TESTS REQUIRED!!!.'''

import math
from materials import concrete_base
from miscUtils import LogMessages as lmsg
from materials import concrete_base

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

aciRCLimitStrains= concrete_base.ReinforcedConcreteLimitStrains(EpsCU= -3.0e-3,EpsC0= -2.0e-3,SMaxStrain= 10e-3);

class ACIConcrete(concrete_base.Concrete):
    """ Concrete model according to ACI 318

    """
    toPascal= 6894.76 #Conversion from Pa to lb/inch2
    fromPascal= 1.0/toPascal #Conversion from lb/inch2 to Pa
    
    def __init__(self,nmbConcrete, fck, gammaC):
        super(ACIConcrete,self).__init__(nmbConcrete,fck, gammaC)
        lmsg.error('Implementation of ACI concrete pending\n.')
    def getEcm(self):
        '''Longitudinal secant modulus of deformation at 28 days expressed
        in [Pa] [+] according to expression 19.2.2.1.b of ACI 318-14.'''
        fcklb_inch2= abs(self.fck*self.fromPascal) #Pa -> lb/inch2
        return 57000*(math.sqrt(fcklb_inch2))*self.toPascal #lb/inch2 -> Pa
    def getEpsc2(self):
        '''
        return strain (<0) at peak stress at parabola-rectangle diagram 
        (figure 6-8 of Portland Cement Association PCA notes on ACI 318-99)
        '''
        return 2.0*0.85*self.fck/self.getEcm()
    def getEpscu2(self):
        """
        return nominal ultimate strain [-] at parabola-rectangle diagram 
        according to clause 22.2.2.1 of ACI 318-14
        """
        return 3.0*(-1e-3)
        
# ACI concretes
A36M= ACIConcrete(nmbConcrete="A36M",fck=-20e6,gammaC=1.5)

# Reinforcing steel.

A706G60= concrete_base.ReinforcingSteel(steelName="A706G60", fyk=420e6, emax=0.08,gammaS=1.15)


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
