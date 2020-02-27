# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to ACI 318. 
VERIFICATION TESTS REQUIRED!!!.'''

from __future__ import print_function
from __future__ import division


import math
from materials import concrete_base
from miscUtils import LogMessages as lmsg
from materials.sections.fiber_section import def_simple_RC_section

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

toPascal= 6894.757 #Conversion from Pa to lb/inch2
fromPascal= 1.0/toPascal #Conversion from lb/inch2 to Pa
pound2Newton= 4.44822 # pound-force to Newton

aciRCLimitStrains= concrete_base.ReinforcedConcreteLimitStrains(EpsCU= -3.0e-3,EpsC0= -2.0e-3,SMaxStrain= 10e-3);

class ACIConcrete(concrete_base.Concrete):
    """ Concrete model according to ACI 318

    :ivar Lambda: modification factor to reflect the reduced mechanical
                  properties of lightweight concrete relative to normalweight
                  concrete of the same compressive strength.
    """    
    def __init__(self,concreteName, fck, gammaC, Lambda= 1.0):
        '''
        Constructor.

        :param concretName: name for the concrete material.
        :param Lambda: modification factor to reflect the reduced mechanical
                       properties of lightweight concrete.
        '''
        super(ACIConcrete,self).__init__(concreteName,fck, gammaC)
        #lmsg.warning('Implementation of ACI concrete pending.')
        self.Lambda= Lambda
    def getEcm(self):
        '''Longitudinal secant modulus of deformation at 28 days expressed
        in [Pa] [+] according to expression 19.2.2.1.b of ACI 318-14.'''
        fcklb_inch2= abs(self.fck*fromPascal) #Pa -> lb/inch2
        return 57000*(math.sqrt(fcklb_inch2))*toPascal #lb/inch2 -> Pa
    def getEpsc2(self):
        '''
        return strain (<0) at peak stress at parabola-rectangle diagram 
        (figure 6-8 of Portland Cement Association PCA notes on ACI 318-99)
        '''
        return 2.0*0.85*self.fck/self.getEcm()
    def getEpscu2(self):
        '''
        return nominal ultimate strain [-] at parabola-rectangle diagram 
        according to clause 22.2.2.1 of ACI 318-14
        '''
        return 3.0*(-1e-3)
    def getSqrtFck(self):
        '''
        return the product: Lambda*math.sqrd(fck).
        '''
        fcklb_inch2= abs(self.fck*fromPascal) #Pa -> lb/inch2
        return toPascal*math.sqrt(fcklb_inch2)
    def getLambdaSqrtFck(self):
        '''
        return the product: Lambda*math.sqrd(fck).
        '''
        return self.Lambda*self.getSqrtFck()
    def getFctm(self):
        """Fctm: mean tensile strength [Pa][+] (according to 
           ACI 318-14 R14.3.2.1 )
        """
        return 5.0*self.getLambdaSqrtFck()
    
        
# ACI concretes
A36M= ACIConcrete(concreteName="A36M",fck=-20e6,gammaC=1.667) #????

c3000= ACIConcrete(concreteName="C3000",fck=-3000*toPascal,gammaC=1.667)
c3500= ACIConcrete(concreteName="C3500",fck=-3500*toPascal,gammaC=1.667)
c4000= ACIConcrete(concreteName="C4000",fck=-4000*toPascal,gammaC=1.667)

# Reinforcing steel.

A615G60= concrete_base.ReinforcingSteel(steelName="A615G60", fyk=415e6, emax=0.08,gammaS=1.15)
A706G60= concrete_base.ReinforcingSteel(steelName="A706G60", fyk=415e6, emax=0.08,gammaS=1.15)
# A108 and A29 material properties per AWS D1.1, 2006, Table 7.1, TypeB
A108= concrete_base.ReinforcingSteel(steelName="A108", fyk=351.63276e6, emax=0.08,gammaS=1.15, k= 65.0/51)
A29= concrete_base.ReinforcingSteel(steelName="A29", fyk=351.63276e6, emax=0.08,gammaS=1.15, k= 65.0/51)


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

def getPsi_sFromBarNumber(barNumber):
    ''' Return factor used to modify development length based on
        reinforcement size according to table 25.4.2.4 of ACI
        318-14.'''
    retval= 1.0
    number= int(barNumber.replace('#',''))
    if(number<7):
        retval= 0.8
    return retval

def getPsi_sFromDiameter(phi):
    ''' Return factor used to modify development length based on
        reinforcement size according to table 25.4.2.4 of ACI
        318-14.

        :param phi: bar diameter.
    '''
    retval= 1.0
    if(phi<standard_bars_diameters['#6']):
        retval= 0.8
    return retval

#Generic layers (rows of rebars)
n2s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#2'],areaRebar= standard_bars_areas['#2'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n2s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#2'],areaRebar= standard_bars_areas['#2'],rebarsSpacing= 0.150,width=1.0,nominalCover=0.050)

n3s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#3'],areaRebar= standard_bars_areas['#3'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n3s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#3'],areaRebar= standard_bars_areas['#3'],rebarsSpacing= 0.150,width=1.0,nominalCover=0.050)

n4s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#4'],areaRebar= standard_bars_areas['#4'],rebarsSpacing= 0.150,width=1.0,nominalCover=0.040)
n4s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#4'],areaRebar= standard_bars_areas['#4'],rebarsSpacing= 0.150,width=1.0,nominalCover=0.050)

n5s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#5'],areaRebar= standard_bars_areas['#5'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n5s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#5'],areaRebar= standard_bars_areas['#5'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

n6s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#6'],areaRebar= standard_bars_areas['#6'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n6s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#6'],areaRebar= standard_bars_areas['#6'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

n7s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#7'],areaRebar= standard_bars_areas['#7'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n7s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#7'],areaRebar= standard_bars_areas['#7'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

n8s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#8'],areaRebar= standard_bars_areas['#8'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n8s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#8'],areaRebar= standard_bars_areas['#8'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

n9s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#9'],areaRebar= standard_bars_areas['#9'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n9s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#9'],areaRebar= standard_bars_areas['#9'],rebarsSpacing= 0.150,width=1.0,nominalCover=0.050)

n10s150r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#10'],areaRebar= standard_bars_areas['#10'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.045)
n10s150r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#10'],areaRebar= standard_bars_areas['#10'],rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

n2s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#2'],areaRebar= standard_bars_areas['#2'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n2s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#2'],areaRebar= standard_bars_areas['#2'],rebarsSpacing= 0.300,width=1.0,nominalCover=0.050)

n3s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#3'],areaRebar= standard_bars_areas['#3'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n3s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#3'],areaRebar= standard_bars_areas['#3'],rebarsSpacing= 0.300,width=1.0,nominalCover=0.050)

n4s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#4'],areaRebar= standard_bars_areas['#4'],rebarsSpacing= 0.300,width=1.0,nominalCover=0.040)
n4s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#4'],areaRebar= standard_bars_areas['#4'],rebarsSpacing= 0.300,width=1.0,nominalCover=0.050)

n5s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#5'],areaRebar= standard_bars_areas['#5'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n5s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#5'],areaRebar= standard_bars_areas['#5'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.050)

n6s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#6'],areaRebar= standard_bars_areas['#6'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n6s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#6'],areaRebar= standard_bars_areas['#6'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.050)

n7s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#7'],areaRebar= standard_bars_areas['#7'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n7s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#7'],areaRebar= standard_bars_areas['#7'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.050)

n8s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#8'],areaRebar= standard_bars_areas['#8'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n8s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#8'],areaRebar= standard_bars_areas['#8'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.050)

n9s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#9'],areaRebar= standard_bars_areas['#9'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n9s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#9'],areaRebar= standard_bars_areas['#9'],rebarsSpacing= 0.300,width=1.0,nominalCover=0.050)

n10s300r45= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#10'],areaRebar= standard_bars_areas['#10'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.045)
n10s300r50= def_simple_RC_section.MainReinfLayer(rebarsDiam=standard_bars_diameters['#10'],areaRebar= standard_bars_areas['#10'],rebarsSpacing= 0.300,width=1.0,nominalCover= 0.050)
