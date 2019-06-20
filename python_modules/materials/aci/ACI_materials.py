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
        lmsg.warning('Implementation of ACI concrete pending.\n')
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
A36M= ACIConcrete(nmbConcrete="A36M",fck=-20e6,gammaC=1.667) #????

c3500= ACIConcrete(nmbConcrete="C3500",fck=-3500*ACIConcrete.toPascal,gammaC=1.667)
c4000= ACIConcrete(nmbConcrete="C4000",fck=-4000*ACIConcrete.toPascal,gammaC=1.667)

# Reinforcing steel.

A615G60= concrete_base.ReinforcingSteel(steelName="A615G60", fyk=420e6, emax=0.08,gammaS=1.15)
A706G60= concrete_base.ReinforcingSteel(steelName="A706G60", fyk=420e6, emax=0.08,gammaS=1.15)


#Bar areas in square meters.

num2Area= 32e-6
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

barAreas= [num2Area, num3Area, num4Area, num5Area, num6Area, num7Area, num8Area, num9Area, num10Area, num11Area, num14Area, num18Area, num18JArea]

diameters= list()

for a in barAreas:
    diameters.append(math.sqrt(4.0*a/math.pi))

standard_bars_areas= dict()
standard_bars_areas['#2']= num2Area
standard_bars_areas['#3']= num3Area
standard_bars_areas['#4']= num4Area
standard_bars_areas['#5']= num5Area
standard_bars_areas['#6']= num6Area
standard_bars_areas['#7']= num7Area
standard_bars_areas['#8']= num8Area
standard_bars_areas['#9']= num9Area
standard_bars_areas['#10']= num10Area
standard_bars_areas['#11']= num11Area
standard_bars_areas['#14']= num14Area
standard_bars_areas['#18']= num18Area
standard_bars_areas['#18J']= num18JArea

def findNumberByArea(area):
    for key in standard_bars_areas:
        barArea= standard_bars_areas[key]
        if(abs(area-barArea)<1e-6):
            return key

standard_bars_diameters= dict()
standard_bars_diameters['#2']= math.sqrt(4.0*num2Area/math.pi)
standard_bars_diameters['#3']= math.sqrt(4.0*num3Area/math.pi)
standard_bars_diameters['#4']= math.sqrt(4.0*num4Area/math.pi)
standard_bars_diameters['#5']= math.sqrt(4.0*num5Area/math.pi)
standard_bars_diameters['#6']= math.sqrt(4.0*num6Area/math.pi)
standard_bars_diameters['#7']= math.sqrt(4.0*num7Area/math.pi)
standard_bars_diameters['#8']= math.sqrt(4.0*num8Area/math.pi)
standard_bars_diameters['#9']= math.sqrt(4.0*num9Area/math.pi)
standard_bars_diameters['#10']= math.sqrt(4.0*num10Area/math.pi)
standard_bars_diameters['#11']= math.sqrt(4.0*num11Area/math.pi)
standard_bars_diameters['#14']= math.sqrt(4.0*num14Area/math.pi)
standard_bars_diameters['#18']= math.sqrt(4.0*num18Area/math.pi)
standard_bars_diameters['#18J']= math.sqrt(4.0*num18JArea/math.pi)

