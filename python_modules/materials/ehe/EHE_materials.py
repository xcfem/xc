# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to EHE-08.'''
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"


import math
import scipy.interpolate
from materials import concrete_base

ReinforcedConcreteLimitStrainsEHE08= concrete_base.ReinforcedConcreteLimitStrains(EpsCU= -3.5e-3,EpsC0= -2.0e-3,SMaxStrain= 10e-3)


class EHEConcrete(concrete_base.Concrete):
    """ Concrete model according to EHE
    
    :ivar nmbConcrete: name of the concrete
    :ivar fck: characteristic strength [Pa]
    :ivar gammaC: concrete partial safety factor
    :ivar typeAggregate: types of aggregate= 
            "cuarcita", "arenisca", "caliza normal", 
            "caliza densa", "volcanica porosa", 
            "volcanica normal", "granito", "diabasa" 
            (defaults to 'cuarcita')

    """

    def __init__(self,nmbConcrete, fck, gammaC,typeAggregate='cuarcita'):
        super(EHEConcrete,self).__init__(nmbConcrete,fck, gammaC)
        self.typeAggregate=typeAggregate
    def getAlphaEcm(self):
        '''Correction coefficient to the longitudinal modulus of deformation 
        taking into account the type of aggregate.
        '''
        if self.typeAggregate=="cuarcita":
            return 1
        elif self.typeAggregate=="arenisca":
            return 0.7
        elif self.typeAggregate=="caliza normal":
            return 0.9
        elif self.typeAggregate=="caliza densa":
            return 1.2
        elif self.typeAggregate=="volcanica porosa":
            return 0.9
        elif self.typeAggregate=="volcanica normal":
            return 1.2
        elif self.typeAggregate=="granito":
            return 1.1
        elif self.typeAggregate=="diabasa":
            return 1.3
        else:
            print 'Error in type of aggregate (Possible choices: "cuarcita", "arenisca", "caliza normal", "caliza densa", "volcanica porosa", "volcanica normal", "granito", "diabasa").'
            return 0.0

    def getEcm(self):
        """Longitudinal secant modulus of deformation at 28 days expressed
        in [Pa] [+] according to clause 39.6 of EHE-08."""
        fcmMPa=abs(self.getFcm())*1e-6
        return 8500*(fcmMPa**(1/3))*1e6

    def getEc0(self):
        '''Initial modulus of longitudinal deformation at 28 days expressed 
        in [Pa] [+] according to clause 39.6 of EHE-08.
        '''
        return self.getAlphaEcm()*self.getEcm()

    def getEcmT(self,t):
        '''Longitudinal concrete modulus of deformation at t days expressed 
        in [Pa] [+] according to comments to clause 39.6 of EHE-08.
        
        :param t: Age of concrete, expressed in days.
        '''
        return math.pow(self.getBetaCC(t),0.3)*self.getEcm()

    def getFctm(self):
        """Fctm: mean tensile strength [Pa][+]
        (art. 39.1 EHE)
        """
        if self.fckMPa()<= 50:
            return 0.3*self.fckMPa()**(2.0/3.0)*1e6
        else:
            return 0.58*(self.fckMPa()**(1.0/2.0))*1e6

    def fctkInf(self):
        """Characteristic tensile strength (lower value) [Pa][+]
        (according to clause 39.1 EHE)
        """
        return 0.21e6*(self.fckMPa())**(2.0/3.0)

    def fctkSup(self):
        """Characteristic tensile strength (higher value) [Pa][+]
        (according to clause 39.1 EHE)
        """
        return 0.39e6*(self.fckMPa())**(2.0/3.0)

    def fctMedEHE08(self):
        """Mean tensile strength [Pa][+]
        (according to clause 39.1 EHE-08)
        """
        if self.fckMPa()<50:
            return 0.30e6*self.fckMPa()**(2/3)
        else:
            return 0.58e6*self.fckMPa()**(1/2)

    def fctkEHE08(self):
        """Concrete characteristic tensile strength  [Pa][+]
        (according to clause 39.1 EHE-08)
        """
        return 0.7*self.fctMedEHE08()

    def getEpsc2(self):
        """
        :return: strain [-] at peak stress at parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return (2.0)*(-1e-3)
        else:
            return (2.0+0.085*(self.fckMPa()-50)**0.50)*(-1e-3)

    def getExpN(self):
        """
        :return: exponent n for the parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return 2.0
        else:
            return 1.4+9.6*((100-self.fckMPa())/100)**4

    def getEpscu2(self):
        """
        :return: nominal ultimate strain [-] at parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return 3.5*(-1e-3)
        else:
            return (2.6+14.4*((100-self.fckMPa())/100)**4)*(-1e-3)

#    def tangDConcrete(self,eps):
#        """Diagrama característico tensión deformación del hormigón según EHE."""
#        return tangc(eps,self.fcd())

#Parabola-rectangle diagrams for concrete under compression 
    # def sigmac(self,eps):
    #     '''Stress [Pa][-] from parabola-rectangle diagram
    #     Atributtes:
    #         eps: strain [-]
    #     '''
    #     if(eps<0):
    #         if(eps>self.getEpsc2()):
    #             return -850*self.fcd()*eps*(250*eps+1)
    #         else:
    #             return 0.85*self.fcd()
    #     else:
    #         return 0.0 

    # def tangc(self,eps):
    #     if(eps<0):
    #       if(eps>self.getEpsc2()):
    #         return (-850*self.fcd()*(500*eps+1)) 
    #       else:
    #         return 0.0
    #     else:
    #       return 0.0 
    def getCreepDeformation(self, t,t0,RH,h0mm, sigmaT0):
        '''
        :Return: the creep deformation between t0 and t
        
        :param t:     age of concrete in days at the moment considered
        :param t0:    age of concrete in days at loading
        :param RH:    ambient relative humidity(%)
        :param h0mm:  notional size of the member in mm 
                   h0mm=2*Ac/u, where:

                       - Ac= cross sectional area
                       - u = perimeter of the member in contact with the atmosphere
                       - sigmaT0: Tensión constante aplicada en t0.
        '''
        return sigmaT0*(1.0/self.getEcmT(t0)+self.getCreepFitt0(t,t0,RH,h0mm)/self.getEcm())
 

# EHE concretes
HA20= EHEConcrete(nmbConcrete="HA20",fck=-20e6,gammaC=1.5)
HA25= EHEConcrete(nmbConcrete="HA25",fck=-25e6,gammaC=1.5)
HA30= EHEConcrete(nmbConcrete="HA30",fck=-30e6,gammaC=1.5)
HA35= EHEConcrete(nmbConcrete="HA35",fck=-35e6,gammaC=1.5)
HA40= EHEConcrete(nmbConcrete="HA40",fck=-40e6,gammaC=1.5)
HA45= EHEConcrete(nmbConcrete="HA45",fck=-45e6,gammaC=1.5)
HA50= EHEConcrete(nmbConcrete="HA50",fck=-50e6,gammaC=1.5)
HA55= EHEConcrete(nmbConcrete="HA55",fck=-55e6,gammaC=1.5)
HA60= EHEConcrete(nmbConcrete="HA60",fck=-60e6,gammaC=1.5)
HA70= EHEConcrete(nmbConcrete="HA70",fck=-70e6,gammaC=1.5)
HA80= EHEConcrete(nmbConcrete="HA80",fck=-80e6,gammaC=1.5)
HA90= EHEConcrete(nmbConcrete="HA90",fck=-90e6,gammaC=1.5)
HA100= EHEConcrete(nmbConcrete="HA100",fck=-100e6,gammaC=1.5)

HP45= EHEConcrete(nmbConcrete="HP45",fck=-55e6,gammaC=1.5)
HP50= EHEConcrete(nmbConcrete="HP50",fck=-50e6,gammaC=1.5)


'''
Factor that multiplies the characteristic value of concrete compressive strength from its age in days for normal hardening concrete (table 30.4.b EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.4,0.65,1.0,1.2,1.35,1.35]
factorRCompJDiasNormal= scipy.interpolate.interp1d(x,y)

'''
Factor that multiplies the characteristic value of concrete compressive strength from its age in days for fast hardening concrete (table 30.4.b EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.55,0.75,1.0,1.15,1.20,1.20]
factorRCompJDiasRapido=  scipy.interpolate.interp1d(x,y)

'''
Factor that multiplies the characteristic value of concrete tensile strength from its age in days for normal hardening concrete (table 30.4.c EHE)
'''
x= [-1.0,0.0,3.0,7.0,28.0,90,360,361]
y= [0.0,0.0,0.4,0.7,1.0,1.05,1.10,1.10]
factorRTraccJDiasNormal= scipy.interpolate.interp1d(x,y)

# ************* Reinforcing steel. ********************

# Maximum strain (0.08 para B400S y 0.05 para B500S)
#
#   steelName: Name identifying the material.
#   fyk:      Characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   Partial factor for material.
#   k:        fmaxk/fyk ratio
B400S= concrete_base.ReinforcingSteel(steelName="B400S", fyk=400e6, emax=0.08,gammaS=1.15)
B500S= concrete_base.ReinforcingSteel(steelName="B500S", fyk=500e6, emax=0.05,gammaS=1.15)

# Bar areas in square meters.
Fi6=0.28e-4
Fi8=0.50e-4
Fi10=0.79e-4
Fi12=1.13e-4
Fi14=1.54e-4
Fi16=2.01e-4
Fi20=3.14e-4
Fi25=4.91e-4
Fi32=8.04e-4
Fi40=12.56e-4

# ************* Prestressing steel. ********************

# Relaxation of steel according to EHE-08 (Artº 38.9)
# and Model Code CEB-FIP 1990.

class EHEPrestressingSteel(concrete_base.PrestressingSteel):
    ''' Prestressing steel model according to EHE-08.

       :param fpk: Elastic limit.
       :param fmax: Steel strength.
       :param alpha: stress-to-strength ratio.
       :param steelRelaxationClass: Relaxation class 1: normal, 2: improved, 
                                    and 3: relaxation for bars.
       :param tendonClass: Tendon class wire, strand or bar.
    '''
    # Points from the table 38.7.a of EHE-08 to determine the 
    # relaxation at 1000 hours, used to estimate
    # the relaxation at times greater than 1000 hours.

    #For wires and strands:
    ptsRO1000Wires= scipy.interpolate.interp1d([0,.5,.6,.7,.8],[0,0,1,2,5.5])
    #For bars:
    ptsRO1000Bars= scipy.interpolate.interp1d([0,.5,.6,.7,.8],[0,0,2,3,7])


    def __init__(self,steelName,fpk,fmax= 1860e6, alpha= 0.75, steelRelaxationClass=1, tendonClass= 'strand'):
        super(EHEPrestressingSteel,self).__init__(steelName,fpk,fmax,alpha,steelRelaxationClass, tendonClass)

    def getRO1000(self):
        '''
        Return the relaxation at 1000 hours after stressing (See table 38.9.a at EHE-08)
        '''
        if(self.tendonClass=="wire"):
            return self.ptsRO1000Wires(self.alpha) 
        elif(self.tendonClass=="strand"):
            return self.ptsRO1000Wires(self.alpha)
        elif(self.tendonClass=="bar"):
            return self.ptsRO1000Bars(self.alpha)

    def getRelaxationT(self, tDias):
        ''' Return the relaxation at time tDias in days after stressing.

        :param tDays: Time in days after prestressing
                  (to make easier to deal with shrinkage and creep at the same time).
        '''
        tHours= tDias*24
        RO1000= self.getRO1000()
        if(tHours<1000):
            return RO1000*self.ptsShortTermRelaxation(tHours)
        else:
            return RO1000*pow(tHours/1000.0,self.getKRelaxation())
    
    def getRelaxationStressLossT(self, tDays, initialStress):
        '''
        Return change in tendon stress due to relaxation at time t.

        :param initialStress: Initial stress in tendon.
        :param tDays: Time in days after prestressing
                  (to make easier to deal with shrinkage and creep at the same time).

        '''
        LGRO120= math.log10(self.getRelaxationT(120/24.0))
        LGRO1000= math.log10(self.getRelaxationT(1000/24.0))
        k2= (LGRO1000-LGRO120)/(3-math.log10(120))
        k1= LGRO1000-3*k2
        tHours= tDays*24
        ROT= pow(10,k1+k2*math.log10(tHours))/100.0
        return initialStress*ROT

    def getRelaxationStressLossFinal(self, initialStress):
        '''
        Return final change in tendon stress due to relaxation.

        :param initialStress: Initial stress in tendon.

        '''
        ROFINAL= 2.9e-2*self.getRelaxationT(1000/24.0)
        return initialStress*ROFINAL

Y1860S7= EHEPrestressingSteel(steelName= "Y1860S7",fpk= 1171e6,fmax= 1860e6)

def get_losses_wedge_penetration_short_straight_tendon(a, L, Ep):
    '''Losses due to wedge penetration in post-tensioned straight
    tendons of short length (according to section 20.2.2.1.2 of EHE-08)

    :param a: wedge penetration (usually: 5 mm)
    :param L: total length of the straight tendon
    :param Ep: longitudinal strain modulus of an active reinforcement (190e9).
    '''
    return a/L*Ep

def get_losses_elastic_shortening_concrete_in_tendons(sigma_cp, Ep, Ecj):
    '''Losses due to elastic shortening of the concrete in prestressed tendons

    :param sigma_cp: compressive stress in the concrete in the level of the 
                     center of gravity of the active reinforcement, due to 
                     prestressing after deducting the losses due to wedge 
                     penetration
    :param Ep: modulus of longitudinal elasticity of the active reinforcements
                (190e9).
    :param Ecj: modulus of longitudinal elasticity of the concrete for the age
                 j corresponding to the moment of applying the load to the 
                 active reinforcements.
    '''
    return sigma_cp*Ep/Ecj
