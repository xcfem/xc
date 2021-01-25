# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to EHE-08.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"


import math
import scipy.interpolate
from materials import concrete_base

ReinforcedConcreteLimitStrainsEHE08= concrete_base.ReinforcedConcreteLimitStrains(EpsCU= -3.5e-3,EpsC0= -2.0e-3,SMaxStrain= 10e-3)


class ReinforcingSteel(concrete_base.ReinforcingSteel):
    ''' Reinforcing steel as defined in EHE-08.

    '''
    # Table 69.5.1.2.a of EHE-08
    x= [25e6,30e6,35e6,40e6,45e6,50e6]
    y400= [1.2,1.0,.9,.8,.7,.7]
    y500= [1.5,1.3,1.2,1.1,1.0,1.0]
    f400= scipy.interpolate.interp1d(x, y400)
    f500= scipy.interpolate.interp1d(x, y500)
    # Table 69.5.2.2 of EHE
    alpha_ratios=[0.0,0.2,0.25,0.33,0.5,0.51,1.0]
    alpha_leq_10phi= [1.0,1.2,1.4,1.6,1.8,2.0,2.0]
    alpha_gt_10phi= [1.0,1.0,1.1,1.2,1.3,1.4,1.4]
    f_alpha_leq_10phi= scipy.interpolate.interp1d(alpha_ratios,alpha_leq_10phi)
    f_alpha_gt_10phi= scipy.interpolate.interp1d(alpha_ratios,alpha_gt_10phi)

    def getM(self, concrete):
        ''' Return the 'm' factor from table 69.5.1.2.a
            of EHE-08.

        :param concrete: concrete material.
        ''' 
        m400= float(self.f400(-concrete.fck))
        m500= float(self.f500(-concrete.fck))
        if(self.fyk==400e6):
            retval= m400
        elif(self.fyk==500e6):
            retval= m500
        else:
            retval= (m400*(500e6-self.fyk)+m500*(self.fyk-400e6))/100e6
        return retval

    def getBasicAnchorageLength(self, concrete, phi, pos, dynamicEffects= False):
        '''Returns basic anchorage length in tension according to clause
           66.5.1.2 of EHE.

        :param concrete: concrete material.
        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        retval= 60.0*phi
        m= self.getM(concrete)
        f= phi*1000.0
        m_phi= m*f
        if(pos=='I'):
            retval= max(m_phi, self.fyk/20e6)*phi
        elif(pos=='II'):
            retval= max(1.4*m_phi,self.fyk/14e6)*phi
        else:
            lmsg.error('position must be I or II')
        if(dynamicEffects):
            retval+= 10*phi
        return retval
    
    def getNetAnchorageLength(self ,concrete, phi, pos, beta= 1.0, efficiency= 1.0, tensionedBars= True, dynamicEffects= False):
        '''Returns net anchorage length in tension according to clause
           6.5.1.2 of EHE.

        :param concrete: concrete material.
        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param beta: reduction factor defined in Table 69.5.1.2.b.
        :param efficiency: working stress of the reinforcement that it is 
                           intended to anchor, on the most unfavourable 
                           load hypothesis, in the section from which 
                           the anchorage length will be determined divided
                           by the steel design yield strength.
        :param tensionedBars: true if the bars are in tension.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        lb= self.getBasicAnchorageLength(concrete, phi, pos, dynamicEffects)
        retval= beta*efficiency*lb
        retval= max(retval, 10.0*phi)
        retval= max(retval, 0.15)
        if(tensionedBars):
            retval= max(retval,lb/3.0)
        else:
            retval= max(retval,2.0*lb/3.0)
        return retval
    
    def getOverlapLength(self ,concrete, phi, pos, distBetweenNearestSplices, beta= 1.0, efficiency= 1.0, ratioOfOverlapedTensionBars= 1.0, tensionedBars= True, dynamicEffects= False):
        '''Returns net anchorage length in tension according to clause
           6.5.1.2 of EHE.

        :param concrete: concrete material.
        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param distBetweenNearestSplices: distance between the nearest splices
                                          according to figure 69.5.2.2.a.
        :param beta: reduction factor defined in Table 69.5.1.2.b.
        :param efficiency: working stress of the reinforcement that it is 
                           intended to anchor, on the most unfavourable 
                           load hypothesis, in the section from which 
                           the anchorage length will be determined divided
                           by the steel design yield strength.
        :param ratioOfOverlapedTensionBars: ratio of overlapped tension bars 
                                            in relation to the total steel
                                            section.
        :param tensionedBars: true if the bars are in tension.
        :param dynamicEffects: true if the anchorage is subjected to
                               dynamic effects.
        '''
        retval= self.getNetAnchorageLength(concrete, phi, pos, beta, efficiency, tensionedBars, dynamicEffects)
        if(distBetweenNearestSplices<10.0*phi):
            alph= float(self.f_alpha_leq_10phi(ratioOfOverlapedTensionBars))
        else:
            alph= float(self.f_alpha_gt_10phi(ratioOfOverlapedTensionBars))
        retval*= alph
        return retval

class EHEConcrete(concrete_base.Concrete):
    """ Concrete model according to EHE
    
    :ivar typeAggregate: types of aggregate= 
            "cuarcita", "arenisca", "caliza normal", 
            "caliza densa", "volcanica porosa", 
            "volcanica normal", "granito", "diabasa" 
            (defaults to 'cuarcita')

    """

    def __init__(self,nmbConcrete, fck, gammaC,typeAggregate='cuarcita'):
        ''' Constructor.

        :param nmbConcrete: name of the concrete
        :param fck: characteristic strength [Pa]
        :param gammaC: concrete partial safety factor
        :param typeAggregate: types of aggregate= 
                "cuarcita", "arenisca", "caliza normal", 
                "caliza densa", "volcanica porosa", 
                "volcanica normal", "granito", "diabasa" 
                (defaults to 'cuarcita')
        '''
        super(EHEConcrete,self).__init__(nmbConcrete,fck, gammaC)
        self.typeAggregate= typeAggregate

    def getM(self, steel):
        ''' Return the "m" coefficient according to table 66.5.1.2.a of
            EHE-08

        :param steel: reinforcing steel.
        '''
        return steel.getM(-self.fck)

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
            lmsg.error('Error in type of aggregate (Possible choices: "cuarcita", "arenisca", "caliza normal", "caliza densa", "volcanica porosa", "volcanica normal", "granito", "diabasa").')
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
        return strain (<0) at peak stress at parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return (2.0)*(-1e-3)
        else:
            return (2.0+0.085*(self.fckMPa()-50)**0.50)*(-1e-3)

    def getExpN(self):
        """
        return exponent n for the parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return 2.0
        else:
            return 1.4+9.6*((100-self.fckMPa())/100)**4

    def getEpscu2(self):
        """
        return nominal ultimate strain [-] at parabola-rectangle diagram 
        (art. 39.5 EHE, figure 39.5.a EHE)
        """
        if self.fckMPa()<=50:
            return 3.5*(-1e-3)
        else:
            return (2.6+14.4*((100-self.fckMPa())/100)**4)*(-1e-3)
        
    def taucd(self):
        '''Design value of shear strength according to artocle 39.1.3.2.2
        of code EHE-91 (f_cv=0.5*sqrt(f_cd)) with f_cd in kp/cm2
        '''
        fcd_kp_cm2=-self.fck*1e-5/self.gmmC
        fcv=0.5*math.sqrt(fcd_kp_cm2)*1e5
        return fcv



#    def tangDConcrete(self,eps):
#        """Characteristic concrete stress-strain diagram according to EHE."""
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
    def getCreepDeformation(self, t,t0,RH,h0, sigmaT0):
        '''
        return the creep deformation between t0 and t
        
        :param t:   age of concrete in days at the moment considered
        :param t0:  age of concrete in days at loading
        :param RH:  ambient relative humidity(%)
        :param h0:  notional size of the member.
 
                       - h0=2*Ac/u, where:
                       - Ac= cross sectional area
                       - u = perimeter of the member in contact with the atmosphere
        :param sigmaT0: Constant stress appliend in t0.
        '''
        return sigmaT0*(1.0/self.getEcmT(t0)+self.getCreepFitt0(t,t0,RH,h0)/self.getEcm())
 

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

HP25= EHEConcrete(nmbConcrete="HP25",fck=-25e6,gammaC=1.5)
HP30= EHEConcrete(nmbConcrete="HP30",fck=-30e6,gammaC=1.5)
HP35= EHEConcrete(nmbConcrete="HP35",fck=-35e6,gammaC=1.5)
HP40= EHEConcrete(nmbConcrete="HP40",fck=-40e6,gammaC=1.5)
HP45= EHEConcrete(nmbConcrete="HP45",fck=-45e6,gammaC=1.5)
HP50= EHEConcrete(nmbConcrete="HP50",fck=-50e6,gammaC=1.5)

concrOfName={"HA20":HA20,"HA25":HA25,"HA30":HA30,"HA35":HA35,"HA40":HA40,"HA45":HA45,"HA50":HA50,"HA55":HA55,"HA60":HA60,"HA70":HA70,"HA80":HA80,"HA90":HA90,"HA100":HA100,"HP25":HP25,"HP30":HP30,"HP35":HP35,"HP40":HP40,"HP45":HP45,"HP50":HP50}

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
factorRTensionJDaysNormal= scipy.interpolate.interp1d(x,y)

# ************* Reinforcing steel. ********************

# Maximum strain (0.08 para B400S y 0.05 para B500S)
#
#   steelName: Name identifying the material.
#   fyk:      Characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   Partial factor for material.
#   k:        fmaxk/fyk ratio
B400S= ReinforcingSteel(steelName="B400S", fyk=400e6, emax=0.08,gammaS=1.15)
B500S= ReinforcingSteel(steelName="B500S", fyk=500e6, emax=0.05,gammaS=1.15)

steelOfName={"B400S":B400S,"B500S":B500S}
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

class PrestressingSteel(concrete_base.PrestressingSteel):
    ''' Prestressing steel model according to EHE-08. '''
    # Points from the table 38.7.a of EHE-08 to determine the 
    # relaxation at 1000 hours, used to estimate
    # the relaxation at times greater than 1000 hours.

    #For wires and strands:
    ptsRO1000Wires= scipy.interpolate.interp1d([0,.5,.6,.7,.8],[0,0,1,2,5.5])
    #For bars:
    ptsRO1000Bars= scipy.interpolate.interp1d([0,.5,.6,.7,.8],[0,0,2,3,7])

    # Table 70.2.3 of EHE
    x= [25e6,30e6,35e6,40e6,45e6,50e6]
    ## For strands
    fpbdStrandValues= [1.4e6,1.6e6,1.8e6,1.9e6,2.1e6,2.2e6]
    fpbdStrand= scipy.interpolate.interp1d(x, fpbdStrandValues)
    ## For wires
    fpbdWireValues= [1.6e6,1.8e6,2.0e6,2.2e6,2.4e6,2.6e6]
    fpbdWire= scipy.interpolate.interp1d(x, fpbdWireValues)

    def __init__(self,steelName,fpk,fmax= 1860e6, alpha= 0.75, steelRelaxationClass= 1, tendonClass= 'strand', Es= 190e9):
        ''' Prestressing steel base class.

           :param steelName: steel name.
           :param fpk: Elastic limit.
           :param fmax: Steel strength.
           :param alpha: stress-to-strength ratio.
           :param steelRelaxationClass: Relaxation class 1: normal, 
                                        2: improved, 
                                        and 3: relaxation for bars.
           :param tendonClass: tendon class: wire, strand or bar.
           :param Es: elastic modulus.
        '''
    
        super(PrestressingSteel,self).__init__(steelName,fpk,fmax,alpha,steelRelaxationClass, tendonClass)
        
    def getDesignAdherenceStress(self, concrete, pos, t= 28):
        ''' Return the design value of the adherence stress according
            to the commentaries to the article 70.2.3 of EHE.

        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        retval= 0.0
        if(t!=28):
            lmsg.error('design value of the adherence stress for concrete age different of 28 days not implemented yet.')
        if(self.tendonClass=='strand'):
            retval= self.fpbdStrand(-concrete.fck)
        elif(self.tendonClass=='wire'):
            retval= self.fpbdWire(-concrete.fck)
        else:
            lmsg.error('prestressed reinforcement type: \''+str(self.tendonClass)+'\' unknown')
        if(pos=='II'):
            retval*= 0.7
        return retval
        
    def getTransmissionLength(self, phi, concrete, pos, sg_pi, suddenRelease= True, ELU= True, t= 28):
        ''' Return the length of transmission for the tendon according
            to the commentaries to the article 70.2.3 of EHE.

        :param phi: nominal diameter of the wire, or prestressing strand.
        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param sg_pi: steel stress just after release.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        alpha1= 1.25
        if(not suddenRelease): # slow prestressing transfer
            alpha1= 1.0
        alpha2= 1.0
        if(not ELU): # Serviceability limit state.
            alpha2= 0.5
        alpha3= 0.5 # strands
        if(self.tendonClass=='wire'):
            alpha3= 0.7 # indented of crimped wires
        fbptdt= self.getDesignAdherenceStress(concrete, pos, t)
        return alpha1*alpha2*alpha3*phi/4.0*sg_pi/fbptdt

    def get_anchorage_length(self, phi, lbpt, concrete, pos, sg_pd, sg_pcs, t):
        ''' Return the design anchorage length for the tendon according
            to the commentaries to the article 70.2.3 of EHE.

        :param phi: nominal diameter of the wire, or prestressing strand.
        :param lbpt: length of transmission for the tendon.
        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param sg_pi: tendon stress just after release.
        :param sg_pd: tendon stress under design load.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        alpha4= 0.8 # strands
        if(self.tendonClass=='wire'):
            alpha4= 1.0 # indented of crimped wires
        fbptdt= self.getDesignAdherenceStress(concrete, pos, t)
        return lbpt+alpha4*phi/4.0*(sg_pd-sg_pcs)/fbptdt
        
    def getAnchorageLength(self, phi, concrete, pos, sg_pi, sg_pd, sg_pcs, suddenRelease= True, ELU= True, t= 28):
        ''' Return the design anchorage length for the tendon according
            to the commentaries to the article 70.2.3 of EHE.

        :param phi: nominal diameter of the wire, or prestressing strand.
        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param sg_pi: tendon stress just after release.
        :param sg_pd: tendon stress under design load.
        :param sg_pcs: tendon stress due to prestress after all losses.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        lbpt= self.getTransmissionLength(phi, concrete, pos, sg_pi, suddenRelease, ELU, t)
        return self.get_anchorage_length(phi, lbpt, concrete, pos, sg_pd, sg_pcs, t)
        
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

Y1860S7= PrestressingSteel(steelName= "Y1860S7",fpk= 1171e6,fmax= 1860e6)

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
    :param Ep: elastic modulus of the active reinforcements (defaults to 190e9).
    :param Ecj: elastic modulus of the concrete for the age
                 j corresponding to the moment of applying the load to the 
                 active reinforcements.
    '''
    return sigma_cp*Ep/Ecj

class Y1860S7Strand(PrestressingSteel):
    ''' Uncoated strand 7-Steel wire for prestressed concrete
        according to EN 0138 - 3: March 2011.

     :ivar diameter: strand diameter.
     :ivar area: cross sectional area.
     '''
    def __init__(self, diameter, area):
        ''' Constructor.

        :param diameter: strand diameter.
        :param area: cross sectional area.
        '''
        super(Y1860S7Strand,self).__init__(steelName= "Y1860S7",fpk= 1171e6,fmax= 1860e6, tendonClass= 'strand')
        self.diameter= diameter
        self.area= area

    def massPerMeter(self):
        ''' Return the mass per meter of the strand.'''
        return self.area*7810.35496123245 # Adjusted value for steel density.

    def Fm(self):
        ''' Return the characteristic valu of maximum force.'''
        return self.area*self.fmax

    def Fm_max(self):
        ''' Return the maximum valu of maximum force.'''
        return self.gammaS*self.Fm()

    def Fp(self):
        return 0.86*self.Fm()

    def getTransmissionLength(self, concrete, pos, sg_pi, suddenRelease= True, ELU= True, t= 28):
        ''' Return the length of transmission for the strand according
            to the commentaries to the article 70.2.3 of EHE.

        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param sg_pi: steel stress just after release.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        return super(Y1860S7Strand,self).getTransmissionLength(self.diameter, concrete, pos, sg_pi, suddenRelease, ELU, t)

    def getAnchorageLength(self, concrete, pos, sg_pi, sg_pd, sg_pcs, suddenRelease= True, ELU= True, t= 28):
        ''' Return the design anchorage length for the strand according
            to the commentaries to the article 70.2.3 of EHE.

        :param concrete: concrete material.
        :param pos: reinforcement position according to clause 66.5.1
                   of EHE-08.
        :param sg_pi: tendon stress just after release.
        :param sg_pd: tendon stress under design load.
        :param sg_pcs: tendon stress due to prestress after all losses.
        :param suddenRelease: if true, prestressing is transfered to concrete
                              in a very short time.
        :param ELU: true if ultimate limit state checking.
        :param t: concrete age at themoment of the prestress transmission
                  expressed in days.
        '''
        lbpt= self.getTransmissionLength(concrete, pos, sg_pi, suddenRelease, ELU, t)
        return self.get_anchorage_length(self.diameter, lbpt, concrete, pos, sg_pd, sg_pcs, t)
    
# Strands mechanical properties.   
Y1860S7Strand_6_9= Y1860S7Strand(diameter= 6.90e-3, area= 29.00e-6)
Y1860S7Strand_7_0= Y1860S7Strand(diameter= 7.00e-3, area= 30.00e-6)
Y1860S7Strand_8_0= Y1860S7Strand(diameter= 8.00e-3, area= 38.00e-6)
Y1860S7Strand_9_0= Y1860S7Strand(diameter= 9.00e-3, area= 50.00e-6)
Y1860S7Strand_9_3= Y1860S7Strand(diameter= 9.30e-3, area= 52.00e-6)
Y1860S7Strand_9_6= Y1860S7Strand(diameter= 9.60e-3, area= 55.00e-6)
Y1860S7Strand_11_0= Y1860S7Strand(diameter= 11.00e-3, area= 70.00e-6)
Y1860S7Strand_11_3= Y1860S7Strand(diameter= 11.30e-3, area= 75.00e-6)
Y1860S7Strand_12_5= Y1860S7Strand(diameter= 12.50e-3, area= 93.00e-6)
Y1860S7Strand_12_9= Y1860S7Strand(diameter= 12.90e-3, area= 100.00e-6)
Y1860S7Strand_13_0= Y1860S7Strand(diameter= 13.00e-3, area= 102.00e-6)
Y1860S7Strand_15_2= Y1860S7Strand(diameter= 15.20e-3, area= 139.00e-6)
Y1860S7Strand_15_3= Y1860S7Strand(diameter= 15.30e-3, area= 140.00e-6)
Y1860S7Strand_15_7= Y1860S7Strand(diameter= 15.70e-3, area= 150.00e-6)

