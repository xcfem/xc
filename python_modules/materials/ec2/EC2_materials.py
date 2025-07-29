# -*- coding: utf-8 -*-
''' Reinforced concrete materials as defined in EC2 
(from Annex C)EHE (spanish RC standard).
 Eurocode 2 permits to use a various steel yielding grades ranging
from 400 MPa to 600 MPa.
Ductility grades:
   -A: cold worked  eps_uk>=2.5% k>=1.05
   -B: hot rolled   eps_uk>=5.0% k>=1.08
   -C: seismic      eps_uk>=7.5% k>=1.15 k<1.35
'''

import sys
import math
from materials import concrete_base
from materials.sections import rebar_family
from misc_utils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

# Concrete according to Eurocode 2.


class EC2Concrete(concrete_base.Concrete):
    """Concrete model according to Eurocode 2:2004

       :ivar typeAggregate: type of aggregate
 
                            - ``Q`` for quartzite aggregate 
                            - ``L`` for limestone aggregate
                            - ``S`` for sandstone aggregate
                            - ``B`` for basalt aggregate

    """
    typeAggregate='Q'
    linearCoefficientOfThermalExpansion= 10e-6 # linear coefficient of thermal expansion according to clause 3.1.3 paragraph (5) of Eurocode 2:2004.

    def __init__(self, nmbConcrete, fck, gammaC, alphacc= 1.0):
        ''' Constructor.

        :param nmbConcrete: material name.
        :param fck: characteristic (5%) cylinder strength of the concrete.
        :param gammaC: partial safety factor for concrete.
        :param alphacc: factor which takes account of the fatigue in the concrete when it is subjected to high levels of compression stress due to long duration loads. Normally alfacc=1 (default value)
        '''
        super(EC2Concrete,self).__init__(nmbConcrete,fck, gammaC, alphacc)
    
    def getEcm(self):
        """
        Ecm: approximate value of the modulus of elasticity [Pa][+] secant value between sigma_c=0 and 0,4fcm, at age 28 days (table 3.1 EC2)
         """
        fcmMPa=abs(self.getFcm())*1e-6
        EcmGPa=22*(fcmMPa/10)**0.3
        if self.typeAggregate == 'L': # limestone.
            return 0.9*EcmGPa*1e9
        elif self.typeAggregate == 'S': # sandstone.
            return 0.7*EcmGPa*1e9
        elif self.typeAggregate == 'B': # basalt.
            return 1.2*EcmGPa*1e9
        else: # quartzite.
            return EcmGPa*1e9
        
    def getEcmT(self, t=28):
        '''EcmT: Value of the modulus of elasticity at an age of t days 
        (sect. 3.1.3 EC2:2004 paragrap (3), expression 3.5)
 
        :param t: age of the concrete in days.
        '''
        return math.pow(self.getFcmT(t)/self.getFcm(), 0.3)*self.getEcm()

    def getEc0(self):
        '''Initial modulus of longitudinal deformation at 28 days expressed 
        in [Pa] according to paragraph (2) of clause 3.1.4 of EC2:2004.
        '''
        return 1.05*self.getEcm()

    def getFctm(self):
        """Fctm: mean tensile strength [Pa][+] (table 3.1 EC2)."""
        if(self.fckMPa()<=50):
            retval= 0.3*math.pow(self.fckMPa(),2.0/3.0)*1e6
        else:
            retval= 2.12*math.log(1+abs(self.getFcm())*1e-6/10)*1e6
        return retval

    def getFctmT(self, t):
        '''FctmT: Value of the tensile strength at an age of t days 
        (sect. 3.1.3 EC2:2004 paragrap (9), expression 3.4)
 
        :param t: age of the concrete in days.
        '''
        alph= 1.0
        if(t>=28):
            alph= 2/3.0
        betacc= self.getBetaCC(t)
        return math.pow(betacc, alph)*self.getFctm()

    def getFlexuralTensileStrength(self, h:float):
        ''' Returns the concrete flexural tensile strength according to
            clause 3.1.8 of EC2:2004. Expression (3.23).

        :param h: total member depth expressed in meters.
        '''
        return max(1.6-h, 1.0)*self.getFctm()

    def getFlexuralTensileStrain(self, h:float):
        ''' Returns the strain corresponding to flexural tensile strength.

        :param h: total member depth.
        '''
        return self.getFlexuralTensileStrength(h)/self.getEcm()
        
    def getEpsc1(self):
        """
        epsc1: strain [-] at peak stress at stress-strain relation 
        (table 3.1 EC2, figure 3.2 EC2)
        """
        fcmMPa=abs(self.getFcm())*1e-6
        return min(0.7*fcmMPa**0.31,2.8)*(-1e-3)

    def getEpscu1(self):
        """
        epscu1: nominal ultimate strain [-]at stress-strain relation 
        (table 3.1 EC2, figure 3.2 EC2)
        """
        fcmMPa=abs(self.getFcm())*1e-6
        if self.fckMPa()<=50:
            return 3.5*(-1e-3)
        else:
            return (2.8+27*((98-fcmMPa)/100)**4)*(-1e-3)

    def getEpsc2(self):
        """
        epsc2: strain [-] at peak stress at parabola-rectangle diagram 
        (table 3.1 EC2, figure 3.3 EC2)
        """
        if self.fckMPa()<=50:
            return 2.0*(-1e-3)
        else:
            return (2.0+0.085*(self.fckMPa()-50)**0.53)*(-1e-3)

    def getExpN(self):
        """
        expN: exponent n for the parabola-rectangle diagram 
        (table 3.1 EC2)
        """
        if self.fckMPa()<=50:
            return 2.0
        else:
            return 1.4+23.4*((90-self.fckMPa())/100)**4

    def getEpscu2(self):
        """
        epscu2: nominal ultimate strain [-] at parabola-rectangle diagram 
        (table 3.1 EC2, figure 3.3 EC2)
        """
        if self.fckMPa()<=50:
            return 3.5*(-1e-3)
        else:
            return (2.6+35*((90-self.fckMPa())/100)**4)*(-1e-3)

    def getEpsc3(self):
        """
        epsc3: strain [-] at peak stress at bi-linear diagram 
        (table 3.1 EC2, figure 3.4 EC2)
        """
        if self.fckMPa()<=50:
            return 1.75*(-1e-3)
        else:
            return (1.75+0.55*(self.fckMPa()-50)/40)*(-1e-3)

    def getEpscu3(self):
        """
        epscu3: nominal ultimate strain [-] at bi-linear diagram 
        (table 3.1 EC2, figure 3.4 EC2)
        """
        fckMPa= self.fckMPa()
        if fckMPa<=50:
            return 3.5*(-1e-3)
        else:
            return (2.6+35*((90-fckMPa)/100)**4)*(-1e-3)

    def getLambdaC(self):
        ''' Return the value of λ according to expressions (3.19) and (3.20)
            of EC2:2004 part 1.'''
        retval= 0.8
        fckMPa= self.fckMPa()
        if(fckMPa>50):
            retval-= (fckMPa-50)/400 # for 50 < fck ≤ 90 MPa
        return retval

    def getEtaC(self):
        ''' Return the value of η according to expressions (3.21) and (3.22)
            of EC2:2004 part 1.
        '''
        retval= 1.0
        fckMPa=  self.fckMPa()
        if(fckMPa>50):
            retval-= (fckMPa-50)/200 # for 50 < fck ≤ 90 MPa
        return retval
        
    def getShearStrengthReductionFactor(self, nationalAnnex= None):
        ''' Return the strength reduction factor for concrete cracked in shear
            according to expression 6.6N of EC2:2004 (clause 6.2.2).
        
        :param nationalAnnex: identifier of the national annex.
        '''
        if((nationalAnnex is None) or (nationalAnnex=='Spain')):
            retval= 0.6*(1-self.fckMPa()/250) # equation 6.6N
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' not implemented yet. Using recommended values.')
            retval= 0.6*(1-self.fckMPa()/250) # equation 6.6N
        return retval
        
    def getMinShearReinfRatio(self, shearReinfSteel, nationalAnnex= None):
        ''' Return the minimum shear reinforcement ratio according
            to expression 9.5N (or the national annex substitution)
        
        :param shearReinfSteel: reinforcing steel material.
        :param nationalAnnex: identifier of the national annex.
        '''
        fyk= shearReinfSteel.fyk # characteristic value of the shear
                                 # reinforcement yield strength.
        if(nationalAnnex is None):
            retval= 0.08*math.sqrt(self.fckMPa())/(fyk/1e6)
        elif(nationalAnnex=='Spain'):
            retval= self.fctm()/7.5/fyk # Spanish national annex.
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' not implemented yet. Using recommended values.')
            retval= 0.08*math.sqrt(self.fckMPa())/(fyk/1e6)
        return retval

    def getAlphaCw(self, NEd, Ac, nationalAnnex= None):
        ''' Return the coefficient taking account of the state of the stress 
            in the compression chord according to expressions 6.11aN, 6.11bN
            and 6.11cN of EC2:2004 (clause 6.2.3).

        :param NEd: axial force in the cross-section due to loading 
                    or prestressing.
        :param Ac: area of concrete cross-section. 
        :param nationalAnnex: identifier of the national annex.
        '''
        sigma_cp= -NEd/Ac/1e6 # concrete compressive stress at the centroidal
                              # axis due to axial loading and/or prestressing
        fcdMPa= -self.fcd()/1e6 # design value of concrete compressive strength (MPa).
        sigma_ratio= sigma_cp/fcdMPa
        if(sigma_cp==0.0):
            retval= 1.0
        elif(sigma_ratio<=0.25):
            retval= 1.0+sigma_ratio
        elif(sigma_ratio<=0.5):
            retval= 1.25
        elif(sigma_ratio<1.0):
            retval= 2.5*(1.0-sigma_ratio)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; excessive concrete stress: '+str(sigma_cp/1e6)+' MPa.')
            retval= 1e-6
        if(nationalAnnex is not None): # something specified.
            if(nationalAnnex!='Spain'): # not spain nor default.
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' not implemented yet. Using recommended values.')
        return retval
    
    def getConcreteFlangeShearStressStrength(self, nationalAnnex= None):
        ''' Return the shear stress resisted by plain concrete according to
            clause 6.2.4 (6) of EC2:2004.

        :param nationalAnnex: identifier of the national annex.
        '''
        k= 0.4 # Depends on national annex.
        if(nationalAnnex is not None): # something specified.
            if(nationalAnnex!='Spain'): # not spain nor default.
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' not implemented yet. Using recommended values.')        
        return k*self.fctd()

    def getUltimateBondStress(self, rebarDiameter, eta1= 0.7):
        ''' Return the ultimate bond stress of the concrete according to
            expression 8.2 of clause 8.4.2 of EC2:2004.

        :param rebarDiameter: bar diameter.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        '''
        if(rebarDiameter<=32e-3):
            eta2= 1.0
        else:
            eta2= (132-rebarDiameter*1e3)/100
        return 2.25*eta1*eta2*self.fctd()
    
    def getCreepDeformation(self, t,t0,RH,h0, sigmaT0):
        '''
        return the creep deformation between t0 and t according
        to clause 5.1 and Annex B of Eurocode 2 part 1-1 : 2004 (Eq. 5.2)
        
        :param t:   age of concrete in days at the moment considered
        :param t0:  age of concrete in days at loading
        :param RH:  ambient relative humidity(%)
        :param h0:  notional size of the member.
 
                       - h0=2*Ac/u, where:
                       - Ac= cross sectional area
                       - u = perimeter of the member in contact with the atmosphere
        :param sigmaT0: constant compressive stress applied on day t0 (negative sign).
        '''
        return self.getCreepFitt0(t,t0,RH,h0)*sigmaT0/self.getEcm()

    def getBearingStrength(self, fbed= None, dryConnection= False):
        ''' Return the bearing strength of the concrete according to paragraph
           (2) of the clause 10.9.5.2 of EN 1992-1-1:2004.

        :param fbed: design strength of bedding material (if none it is 
                     supposed to be greater than the strength of the bearing
                     concrete).
        :param dryConnection: set to true when the connection corresponds to
                              a dry one according to paragrah (3) of the clause
                              10.9.4.3 of EN 1992-1-1:2004.
        '''
        fcd= self.fcd()
        if(dryConnection):
            retval= 0.4*fcd
        else:
            retval= 0.85*fcd
            if(fbed is not None):
                retval= min(retval, fbed)
        return retval
            
        
#    def getEcmT(self):
#        """
#        EC2Ecmt: approximate value of the modulus of elasticity [Pa] at age 
#        t days, according to Eurocode2
#        """
#        return (self.getFcmT()/self.getFcm())**0.3*self.getEcm()


class EC2Concrete2021(EC2Concrete):
    """Concrete model according to Eurocode 2:2021
    """

    def __init__(self, nmbConcrete, fck, gammaC):
        ''' Constructor.

        :param nmbConcrete: concrete name.
        :param fck: characteristic (5%) cylinder strength of the concrete.
        :param gammaC: partial safety factor for concrete.
        '''
        super(EC2Concrete2021,self).__init__(nmbConcrete,fck, gammaC)

    # Autogenous shrinkage strain
    def getShrAlphabs(self):
        '''value of the alpha_bs coefficient accordint to table B.3 of 
           Eurocode2:2021 part 1-1.
        '''
        if self.cemType=='R': # high early strength
            retval= 600
        elif self.cemType=='S': # low early strength
            retval= 800
        else: # ordinary early strength
            retval= 700
        return retval
    
    def getShrEpscbsfcm(self):
        '''notional basic shrinkage coefficient according to expression
           according to expression B26 of clause B.6 of Eurocode 2:2021
           part 1-1.
        '''
        alpha_bs= self.getShrAlphabs()
        fcm28= abs(self.getFcm())
        return alpha_bs*math.pow(fcm28/(60e6+fcm28),2.5)*-1e-6
        
    def getShrEpscbs(self,t, alpha_ndp_b= 1.0):
        '''Basic shrinkage strain according to expression B.24 of 
           clause B.6 Eurocode 2:2021 part 1-1.

        :param t: age of concrete in days at the moment considered
        :param alpha_ndp_b: 1.0 unless otherwise stated in the national annex.
        '''
        retval= self.getShrEpscbsfcm()
        retval*= self.getShrBetaast(t)
        retval*= alpha_ndp_b
        return retval
        
    # Drying shrinkage strain
    def getShrAlphads(self):
        '''value of the alpha_ds coefficient according to table B.3 of 
           Eurocode2:2021 part 1-1.
        '''
        if self.cemType=='R':    #high early strength
            retval= 6
        elif self.cemType=='S':  #low early strength
            retval= 3
        else:                   # ordinary early strength
            retval= 4
        return retval
    
    def getShrRHeq(self):
        '''value of the internal relative humidity of concrete (%) at
           equilibrium according to expression B30 of clause B.6 of
           Eurocode2:2021 part 1-1.
        '''
        return 99*min(math.pow(35e6/abs(self.getFcm()),.1),1.0)

    def getShrBetaRH(self, RH):
        '''coefficient to consider the effect of relative humidity on drying
           shrinkage according to expression B29 of clause B.6 of
           Eurocode2:2021 part 1-1.

        :param RH: relative humidity (%).
        '''
        rhEq= self.getShrRHeq()
        frac= RH/rhEq
        retval= -1.55
        if(RH<20):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; relative humidity: '+str(self.HR)+' out of range (20%<=HR<=100%).')
        elif(RH<=rhEq): # equation B.29a
            retval*= (1-frac**3)
        elif(RH<=100):
            retval*= (1-frac**2)
            if(retval==100): retval+= 25
        else: # RH>100 impossible.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; relative humidity: '+str(self.HR)+' out of range (20%<=HR<=100%).')        
        return retval

    def getShrBetadstts(self, t, ts, h0):
        '''time development of drying shrinkage according to expression B31
           of clause B.6 of Eurocode2:2021 part 1-1.

        :param t: age of concrete in days at the moment considered
        :param ts: age of concrete in days at the beginning of drying shrinkage (or swelling). Normally this is at the end of curing
        :param h0: notional size of the member.
               - h0=``2*Ac/u``, where:
               - Ac= cross sectional area
               - u = perimeter of the member in contact with the atmosphere
        '''
        if t<ts:
            shrBetadstts=0.0
        else:
            shrBetadstts=math.pow((t-ts)/(0.035*((h0*1e3)**2)+(t-ts)),0.5)
        return shrBetadstts
    
    def getShrEpscdsfcm(self):
        '''notional drying shrinkage coefficient according to expression
           B28 of clause B.6 of Eurocode 2:2021 part 1-1.
        '''
        alpha_ds= self.getShrAlphads()
        fcm28= abs(self.getFcm())/1e6
        return (-(220+110*alpha_ds)/math.exp(0.012*fcm28))*1e-6
    
    def getShrEpscds(self,t, ts, h0, RH, alpha_ndp_d= 1.0):
        '''Drying shrinkage strain according to expression B.25 of 
           clause B.6 Eurocode 2:2021 part 1-1.

        :param t: age of concrete in days at the moment considered
        :param ts: age of concrete in days at the beginning of drying shrinkage (or swelling). Normally this is at the end of curing
        :param h0: notional size of the member.
               - h0=``2*Ac/u``, where:
               - Ac= cross sectional area
               - u = perimeter of the member in contact with the atmosphere
        :param RH: ambient relative humidity(%)
        :param alpha_ndp_d: 1.0 unless otherwise stated in the national annex.
        '''
        if t<ts:
            retval=0.0
        else:
            retval= self.getShrEpscdsfcm()
            retval*= self.getShrBetaRH(RH)
            retval*= self.getShrBetadstts(t= t, ts= ts, h0= h0)
            retval*= -alpha_ndp_d
        return retval

    def getShrEpscs(self,t, ts, RH, h0, alpha_ndp_b= 1.0, alpha_ndp_d= 1.0):
        '''Return the total mean shrinkage strain according to expression B.23 
           of clause B.6 Eurocode 2:2021 part 1-1.

        :param t:     age of concrete in days at the moment considered
        :param ts:    age of concrete in days at the beginning of drying shrinkage (or swelling)
                   Normally this is at the end of curing
        :param RH:    ambient relative humidity(%)
        :param h0:  notional size of the member.
                  - h0= 2*Ac/u, where:
                  - Ac= cross sectional area
                  - u = perimeter of the member in contact with the atmosphere
        :param alpha_ndp_b: 1.0 unless otherwise stated in the national annex.
        :param alpha_ndp_d: 1.0 unless otherwise stated in the national annex.
        '''
        return self.getShrEpscds(t= t,ts= ts, h0= h0, RH= RH)+self.getShrEpscbs(t= t)

    
#EC2 concretes 
#1.5: recommended partial factor for concrete in persistent and transient 
#design situations. In accidental situations gammac=1.2 is recommended
C12= EC2Concrete("C12-15",-12e6,1.5)
C16= EC2Concrete("C16-20",-16e6,1.5)
C20= EC2Concrete("C20-25",-20e6,1.5)
C25= EC2Concrete("C25-30",-25e6,1.5)
C30= EC2Concrete("C30-37",-30e6,1.5)
C35= EC2Concrete("C35-45",-35e6,1.5)
C40= EC2Concrete("C40-50",-40e6,1.5)
C45= EC2Concrete("C45-55",-45e6,1.5)
C50= EC2Concrete("C50-60",-50e6,1.5)
C55= EC2Concrete("C55-67",-55e6,1.5)
C60= EC2Concrete("C60-75",-60e6,1.5)
C70= EC2Concrete("C70-85",-70e6,1.5)
C80= EC2Concrete("C80-95",-80e6,1.5)
C90= EC2Concrete("C90-105",-90e6,1.5)
concrOfName={"C12-15":C12,"C16-20":C16,"C20-25":C20,"C25-30":C25,"C30-37":C30,"C35-45":C35,"C40-50":C40,"C45-55":C45,"C50-60":C50,"C55-67":C55,"C60-75":C60,"C70-85":C70,"C80-95":C80,"C90-105":C90}


# EC2 reinforcing steel.

class ReinforcingSteel(concrete_base.ReinforcingSteel):
    ''' Reinforcing steel as defined in EC2:2004.

    '''
    def getBasicAnchorageLength(self, concrete, rebarDiameter, eta1= 0.7, steelEfficiency= 1.0):
        '''Returns basic required anchorage length in tension according to 
           clause 8.4.3 of EC2:2004 (expression 8.3).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        '''
        fbd= concrete.getUltimateBondStress(rebarDiameter= rebarDiameter, eta1= eta1)
        sigma_sd= steelEfficiency*self.fyd()
        return (rebarDiameter/4.0)*sigma_sd/fbd
    
    def getMinimumAnchorageLength(self, lb_rqd, rebarDiameter, compression= True):
        ''' Return the minimum anchorage length according to expressions
            8.6 and 8.7 of EC2:2004 clause 8.4.4.

        :param lb_rqd: basic required anchorage length in tension according to 
                       clause 8.4.3 of EC2:2004 (expression 8.3).
        :param rebarDiameter: nominal diameter of the bar.
        :param compression: if true return the minimum anchorage length for
                            anchorages in compression.
        '''
        if(compression):
            factor= 0.6
        else:
            factor= 0.3
        return max(factor*lb_rqd,max(10*rebarDiameter, 0.1))
                
    def getDesignAnchorageLength(self, concrete, rebarDiameter, eta1= 0.7, steelEfficiency= 1.0, compression= True, alpha_1= 1.0, alpha_2= 1.0, alpha_3= 1.0, alpha_4= 1.0, alpha_5= 1.0):
        '''Returns design  anchorage length according to clause 8.4.4
           of EC2:2004 (expression 8.4).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        :param compression: if true return the minimum anchorage length for
                            anchorages in compression.
        :param alpha_1: effect of the form of the bars assuming adequate cover.
        :param alpha_2: effect of concrete minimum cover.
        :param alpha_3: effect of confinement by transverse reinforcement.
        :param alpha_4: influence of one or more welded transverse bars along 
                        the design anchorage length.
        :param alpha_5: effect of the pressure transverse to the plane of 
                        splitting along the design anchorage length.
        '''
        lb_rqd= self.getBasicAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= eta1, steelEfficiency= steelEfficiency)
        lb_min= self.getMinimumAnchorageLength(lb_rqd= lb_rqd, rebarDiameter= rebarDiameter, compression= compression)
        factor= alpha_1*max(0.7, alpha_2*alpha_3*alpha_5)*alpha_4
        return max(lb_min, factor*lb_rqd)

    def getAlpha6(self, ratioOfOverlapedTensionBars= 1):
        ''' Return the value of the coefficient alpha_6 according to table
            8.3 of EC2:2004.

        :param ratioOfOverlapedTensionBars: ratio of overlapped tension bars 
                                            in relation to the total steel
                                            section.
        '''
        retval= math.sqrt(ratioOfOverlapedTensionBars/0.25)
        return min(max(retval, 1.0), 1.5)        
    
    def getLapLength(self, concrete, rebarDiameter, eta1= 0.7, steelEfficiency= 1.0, ratioOfOverlapedTensionBars= 1.0, alpha_1= 1.0, alpha_2= 1.0, alpha_3= 1.0, alpha_5= 1.0):
        '''Returns the value of the design lap length according to clause
           8.7.3 of EC2:2004 (expression 8.10).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        :param ratioOfOverlapedTensionBars: ratio of overlapped tension bars 
                                            in relation to the total steel
                                            section.
        :param alpha_1: effect of the form of the bars assuming adequate cover.
        :param alpha_2: effect of concrete minimum cover.
        :param alpha_3: effect of confinement by transverse reinforcement.
        :param alpha_5: effect of the pressure transverse to the plane of 
                        splitting along the design anchorage length.
        '''
        lb_rqd= self.getBasicAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= eta1, steelEfficiency= steelEfficiency)
        alpha_6= self.getAlpha6(ratioOfOverlapedTensionBars)
        l0= alpha_1*alpha_2*alpha_3*alpha_5*alpha_6*lb_rqd
        l0_min= max(0.3*alpha_6*lb_rqd, 15*rebarDiameter, 0.2)
        return max(l0, l0_min)

#
#   steelName: Name identifying the material.
#   fyk:      characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   partial factor for material.
#   k:        fmaxk/fyk ratio
S400A= ReinforcingSteel(steelName="S400A", fyk=400e6, emax=0.025,gammaS=1.15,k=1.05)
S400B= ReinforcingSteel(steelName="S400B", fyk=400e6, emax=0.05,gammaS=1.15,k=1.08)
S400C= ReinforcingSteel(steelName="S400C", fyk=400e6, emax=0.075,gammaS=1.15,k=1.15)

S450A= ReinforcingSteel(steelName="S450A", fyk=450e6, emax=0.025,gammaS=1.15,k=1.05)
S450B= ReinforcingSteel(steelName="S450B", fyk=450e6, emax=0.05,gammaS=1.15,k=1.08)
S450C= ReinforcingSteel(steelName="S450C", fyk=450e6, emax=0.075,gammaS=1.15,k=1.15)

S500A= ReinforcingSteel(steelName="S500A", fyk=500e6, emax=0.025,gammaS=1.15,k=1.05)
S500B= ReinforcingSteel(steelName="S500B", fyk=500e6, emax=0.05,gammaS=1.15,k=1.08)
S500C= ReinforcingSteel(steelName="S500C", fyk=500e6, emax=0.075,gammaS=1.15,k=1.15)

S550A= ReinforcingSteel(steelName="S550A", fyk=550e6, emax=0.025,gammaS=1.15,k=1.05)
S550B= ReinforcingSteel(steelName="S550B", fyk=550e6, emax=0.05,gammaS=1.15,k=1.08)
S550C= ReinforcingSteel(steelName="S550C", fyk=550e6, emax=0.075,gammaS=1.15,k=1.15)

S600A= ReinforcingSteel(steelName="S600A", fyk=600e6, emax=0.025,gammaS=1.15,k=1.05)
S600B= ReinforcingSteel(steelName="S600B", fyk=600e6, emax=0.05,gammaS=1.15,k=1.08)
S600C= ReinforcingSteel(steelName="S600C", fyk=600e6, emax=0.075,gammaS=1.15,k=1.15)

steelOfName={"S400A":S400A,"S400B":S400B,"S400C":S400C,"S450A":S450A,"S450B":S450B,"S450C":S450C,"S500A":S500A,"S500B":S500B,"S500C":S500C,"S550A":S550A,"S550B":S550B,"S550C":S550C,"S600A":S600A,"S600B":S600B,"S600C":S600C}

# Bar areas in square meters.
Fi6= 0.28e-4
Fi8= 0.50e-4
Fi10= 0.79e-4
Fi12= 1.13e-4
Fi14= 1.54e-4
Fi16= 2.01e-4
Fi20= 3.14e-4
Fi25= 4.91e-4
Fi32= 8.04e-4
Fi40= 12.56e-4

# rebars as defined in Eurocode 2.
rebarsEC2={}
rebars= rebarsEC2 # useful alias.

rebarsEC2['fi04']= {'nmb':'fi04','d':4e-3,'P':0.099}
rebarsEC2['fi05']= {'nmb':'fi05','d':5e-3,'P':0.154}
rebarsEC2['fi06']= {'nmb':'fi06','d':6e-3,'P':0.222}
rebarsEC2['fi07']= {'nmb':'fi07','d':7e-3,'P':0.302}
rebarsEC2['fi08']= {'nmb':'fi08','d':8e-3,'P':0.395}
rebarsEC2['fi10']= {'nmb':'fi10','d':10e-3,'P':0.617}
rebarsEC2['fi12']= {'nmb':'fi12','d':12e-3,'P':0.888}
rebarsEC2['fi14']= {'nmb':'fi14','d':14e-3,'P':1.208}
rebarsEC2['fi16']= {'nmb':'fi16','d':16e-3,'P':1.578}
rebarsEC2['fi20']= {'nmb':'fi20','d':20e-3,'P':2.466}
rebarsEC2['fi25']= {'nmb':'fi25','d':25e-3,'P':3.854}
rebarsEC2['fi32']= {'nmb':'fi32','d':32e-3,'P':6.313}
rebarsEC2['fi40']= {'nmb':'fi40','d':40e-3,'P':9.865}
rebarsEC2['fi50']= {'nmb':'fi50','d':50e-3,'P':15.413}


for item in rebarsEC2:
    bar= rebarsEC2[item]
    bar['r']= bar['d']/2.0
    bar['area']= math.pi*bar['r']*bar['r']
    
rebar04_S400B= rebar_family.Rebar(diam= 4e-3, steel= S400B) 
rebar06_S400B= rebar_family.Rebar(diam= 6e-3, steel= S400B) 
rebar08_S400B= rebar_family.Rebar(diam= 8e-3, steel= S400B) 
rebar10_S400B= rebar_family.Rebar(diam= 10e-3, steel= S400B) 
rebar12_S400B= rebar_family.Rebar(diam= 12e-3, steel= S400B) 
rebar16_S400B= rebar_family.Rebar(diam= 16e-3, steel= S400B) 
rebar20_S400B= rebar_family.Rebar(diam= 20e-3, steel= S400B) 
rebar25_S400B= rebar_family.Rebar(diam= 25e-3, steel= S400B) 
rebar32_S400B= rebar_family.Rebar(diam= 32e-3, steel= S400B) 
rebar40_S400B= rebar_family.Rebar(diam= 40e-3, steel= S400B) 
rebar50_S400B= rebar_family.Rebar(diam= 50e-3, steel= S400B) 

rebar04_S500B= rebar_family.Rebar(diam= 4e-3, steel= S500B) 
rebar06_S500B= rebar_family.Rebar(diam= 6e-3, steel= S500B) 
rebar08_S500B= rebar_family.Rebar(diam= 8e-3, steel= S500B) 
rebar10_S500B= rebar_family.Rebar(diam= 10e-3, steel= S500B) 
rebar12_S500B= rebar_family.Rebar(diam= 12e-3, steel= S500B) 
rebar16_S500B= rebar_family.Rebar(diam= 16e-3, steel= S500B) 
rebar20_S500B= rebar_family.Rebar(diam= 20e-3, steel= S500B) 
rebar25_S500B= rebar_family.Rebar(diam= 25e-3, steel= S500B) 
rebar32_S500B= rebar_family.Rebar(diam= 32e-3, steel= S500B) 
rebar40_S500B= rebar_family.Rebar(diam= 40e-3, steel= S500B) 
rebar50_S500B= rebar_family.Rebar(diam= 50e-3, steel= S500B) 



#       ___            _                 _           
#      | _ \_ _ ___ __| |_ _ _ ___ _____(_)_ _  __ _ 
#      |  _/ '_/ -_|_-<  _| '_/ -_|_-<_-< | ' \/ _` |
#      |_| |_| \___/__/\__|_| \___/__/__/_|_||_\__, |
#       __| |_ ___ ___| |                      |___/ 
#      (_-<  _/ -_) -_) |                            
#      /__/\__\___\___|_|                            
# ************* Prestressing steel. ********************
# ******** According to EN 1992-1-1:2004 ***************
# Applicable clauses:
# 3.3 Prestressing steel
# 5.10 Prestressed members and structures
# 10.3.2 Prestressing steel
class PrestressingSteel(concrete_base.PrestressingSteel):
    ''' Prestressing steel model according to EC2.

    :ivar euk: elongation at maximum load.
    '''
    def __init__(self,steelName, fp01k, euk= None, fmax= 1860e6, alpha= 0.75, steelRelaxationClass= 1, tendonClass= 'strand', Es= 195e9):
        ''' Prestressing steel base class.

        :param steelName: steel name.
        :param fp01k: Characteristic 0,1% proof-stress of prestressing steel.
        :param euk: elongation at maximum load.
        :param fmax: Tensile strength.
        :param alpha: initial stress-to-strength ratio.
        :param steelRelaxationClass: Relaxation class 1: normal, 
                                     2: improved, 
                                     and 3: relaxation for bars.
        :param tendonClass: tendon class: wire, strand or bar.
        :param Es: elastic modulus (see clause 3.3.6 paragraphs (2) and (3).
        '''
        super(PrestressingSteel,self).__init__(steelName= steelName, fpk= fp01k, fmax= fmax, alpha= alpha, steelRelaxationClass= steelRelaxationClass, tendonClass= tendonClass, Es= Es)
        if(euk):
            self.euk= euk
        else:
            self.euk= 0.02/0.9 # See clause 3.3.6 paragraph (7)

    def get_fp01k(self):
        '''Return the characteristic value of the 0.1% proof-stress of the
           prestressing steel.'''
        return self.fpk

    def get_fpk(self):
        '''Return the characteristic value of the tensile strength of the
           prestressing steel.
        '''
        return self.fmax

    def getMaximumStressingStress(self, overStressing= False, nationalAnnex= None):
        ''' Return the maximum prestressing stress (i.e. the stress at the 
            active end during tensioning) according to clause 5.10.2.1 of 
            EN 1992-1-1:2004.

        :param overStressing: apply paragraph (2) of clause 5.10.2.1.
        :param nationalAnnex: identifier of the national annex.
        '''
        if(overStressing): # see 5.10.2.1 (2)
            if((nationalAnnex is None) or (nationalAnnex=='Germany')):
                k3= 0.95
            elif(nationalAnnex=='Spain'): 
                k3= 0.90
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' unknown. Using recommended values.')
            retval= k3*self.get_fp01k()
        else: # see 5.10.2.1 (1)P
            if((nationalAnnex is None) or (nationalAnnex=='Germany')):
                k1= 0.8
                k2= 0.9
            elif(nationalAnnex=='Spain'): 
                k1= 0.75
                k2= 0.85             
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' unknown. Using recommended values.')
            retval= min(k1*self.get_fpk(), k2*self.get_fp01k())
        return retval
    
    def getMaximumStressingForce(self, Ap, overStressing= False, nationalAnnex= None):
        ''' Return the maximum prestressing force (i.e. the force at the active
            end during tensioning) according to clause 5.10.2.1 of 
            EN 1992-1-1:2004.

        :param Ap: cross-sectional area of the tendon.
        :param overStressing: apply paragraph (2) of clause 5.10.2.1.
        :param nationalAnnex: identifier of the national annex.
        '''
        return Ap*self.getMaximumStressingStress(overStressing= overStressing, nationalAnnex= nationalAnnex)
    
    def getMaximumInitialStress(self, nationalAnnex= None):
        ''' Return the maximum initial stress (i.e. the stress in the tendon
            immediately after tensioning or transfer) according to clause 
            5.10.3(2) of EN 1992-1-1:2004.

        :param nationalAnnex: identifier of the national annex.
        '''
        if((nationalAnnex is None) or (nationalAnnex in ['Germany', 'Spain'])):
            k7= 0.75
            k8= 0.85
        else:
            k7= 0.75
            k8= 0.85
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; national annex: '+str(nationalAnnex)+' unknown. Using recommended values.')
        retval= min(k7*self.get_fpk(), k8*self.get_fp01k())
        return retval
    
    def getMaximumInitialForce(self, Ap, nationalAnnex= None):
        ''' Return the maximum initial force (i.e. the force in the tendon
            immediately after tensioning or transfer) according to clause 
            5.10.3(2) of EN 1992-1-1:2004.

        :param Ap: cross-sectional area of the tendon(s).
        :param nationalAnnex: identifier of the national annex.
        '''
        return Ap*self.getMaximumInitialStress(nationalAnnex= nationalAnnex)

    def getTimeDependentStressLosses(self, Ap, concrete, Ac, Ic, z_cp, epsilon_cs, fi_t_t0, delta_sigma_pr, sigma_c_qp):
        ''' Return the time dependent losses of prestress according to clause
            5.10.6 od of EN 1992-1-1:2004 (expression (5.46))

        :param Ap: cross-sectional area of the tendon(s).
        :param concrete: concrete material.
        :param Ac: area of the concrete section.
        :param Ic: second moment of area of the concrete section.
        :param z_cp: distance between the centre of gravity of the concrete
                     section and the tendons.
        :param epsilon_cs: estimated shrinkage strain according to 3.1.4(6).
        :param fi_t_t0: creep coefficient at a time t and load application 
                        at time t0.
        :param delta_sigma_pr: absolute value of the variation of stress in 
                               the tendons at location x, at time t, due to 
                               the relaxation of the prestressing steel.
        :param sigma_c_qp: stress in the concrete adjacent to the tendons, due
                           to self-weight and initial prestress and other 
                           quasi-permanent actions where relevant.
        '''
        Ep= self.Es
        Ecm= concrete.Ecm()
        Ep_Ecm= Ep/Ecm
        num= epsilon_cs*Ep+0.8*delta_sigma_pr+Ep_Ecm*fi_t_t0*sigma_c_qp
        denom= 1+Ep_Ecm*Ap/Ac*(1+Ac/Ic*z_cp**2)*(1+0.8*fi_t_t0)
        return num/denom
    
    def getTimeDependentForceLosses(self, Ap, concrete, Ac, Ic, z_cp, epsilon_cs, fi_t_t0, delta_sigma_pr, sigma_c_qp):
        ''' Return the time dependent losses of prestress according to clause
            5.10.6 of EN 1992-1-1:2004.

        :param Ap: cross-sectional area of the tendon(s).
        :param concrete: concrete material.
        :param Ac: area of the concrete section.
        :param Ic: second moment of area of the concrete section.
        :param z_cp: distance between the centre of gravity of the concrete
                     section and the tendons.
        :param epsilon_cs: estimated shrinkage strain according to 3.1.4(6).
        :param fi_t_t0: creep coefficient at a time t and load application 
                        at time t0.
        :param delta_sigma_pr: absolute value of the variation of stress in 
                               the tendons at location x, at time t, due to 
                               the relaxation of the prestressing steel.
        :param sigma_c_qp: stress in the concrete adjacent to the tendons, due
                           to self-weight and initial prestress and other 
                           quasi-permanent actions where relevant.
        '''
        retval= self.getTimeDependentStressLosses(Ap= Ap, concrete= concrete, Ac= Ac, Ic= Ic, z_cp= z_cp, epsilon_cs= epsilon_cs, fi_t_t0= fi_t_t0, delta_sigma_pr= delta_sigma_pr, sigma_c_qp= sigma_c_qp)
        retval*= Ap
        return retval

    def getMu(self, sigma_pi):
        ''' Return the value of mu= sigma_pi/fpk according to clause 3.3.2(5)
            of EN 1992-1-1:2004.

        :param sigma_pi: for post-tensioning sigma_pi is the value of the
                         initial prestress.
        '''
        return max(sigma_pi/self.get_fpk(), 0.55)

    def getRo1000(self):
        ''' Return the value for ro_1000 according to paragraph (6) of clause
            3.3.2 of EN 1992-1-1:2004.
        '''
        if(self.steelRelaxationClass==1):
            retval= 0.08
        elif(self.steelRelaxationClass==2):
            retval= 0.025
        elif(self.steelRelaxationClass==3):
            retval= 0.04
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; relaxation class: '+str(self.steelRelaxationClass)+' unknown. Returning none.')
            retval= None
        return retval

    def getRelaxationLoss(self, sigma_pi, t, ro1000= None):
        ''' Return the value of the relaxation loss according to clause 3.3.2(7)
            of EN 1992-1-1:2004.

        :param sigma_pi: for post-tensioning sigma_pi is the value of the
                         initial prestress.
        :param t: time after tensioning (in hours).
        '''
        if(ro1000 is None):
            ro1000= 100.0*self.getRo1000()
        retval= ro1000
        mu= self.getMu(sigma_pi)
        if(self.steelRelaxationClass==1):
            retval*= 5.39; b= 6.7
        elif(self.steelRelaxationClass==2):
            retval*= 0.66; b=9.1
        elif(self.steelRelaxationClass==3):
            retval*= 1.98; b= 8
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; relaxation class: '+str(self.steelRelaxationClass)+' unknown. Returning none.')
            retval= None
        if(retval):
            retval*= math.exp(b*mu)*math.pow(t/1000, 0.75*(1-mu))*1e-5
            retval*= sigma_pi
        return retval

    def getUltimateBondStress(self, concrete, bondConditionsCoeff= 0.7, tendonTypeCoeff= 1.2):
        ''' Return the bond strength for anchorage according to expression
            (8.20) of to clause 8.10.2.3(2) of EN 1992-1-1:2004.

        :param concrete: concrete material.
        :param tendonTypeCoeff: coefficient that takes into account the type 
                                of tendon and the bond situation at anchorage
                                (1.4 for indented wires and 1.2 for 7-wire
                                strands.
        :param bondConditionsCoeff: 1.0 for good bond conditions (see 8.4.2)
                                    0.7 otherwise.
        '''
        return bondConditionsCoeff*tendonTypeCoeff*concrete.fctd()
    
    def getAnchorageLength(self, transmissionLength, concrete, tendonDiameter, sigma_pd, sigma_pminf, bondConditionsCoeff= 0.7, tendonTypeCoeff= 1.2, alpha2= 0.25):
        ''' Return the bond strength for anchorage according to expression
            (8.20) of to clause 8.10.2.3(2) of EN 1992-1-1:2004.

        :param transmisionLength: upper design value of transmission length, 
                                  see 8.10.2.2 (3).
        :param concrete: concrete material.
        :param tendonDiameter: diameter of the tendon.
        :param sigma_pd: tendon stress corresponding to the force described in
                         paragraph (1) of clause 8.10.2.3.
        :param sigma_pminf: prestress after all losses.
        :param tendonTypeCoeff: coefficient that takes into account the type 
                                of tendon and the bond situation at anchorage
                                (1.4 for indented wires and 1.2 for 7-wire
                                strands.
        :param bondConditionsCoeff: 1.0 for good bond conditions (see 8.4.2)
                                    0.7 otherwise.
        :param alpha2: 0.25 for tendons with circular corss section, and 0.19
                       for 3 and 7-wire strands.
        '''
        fbpd= self.getUltimateBondStress(concrete= concrete, bondConditionsCoeff= bondConditionsCoeff, tendonTypeCoeff= tendonTypeCoeff)
        return transmissionLength+alpha2*tendonDiameter*(sigma_pd-sigma_pminf)/fbpd
        

    
        
        
# Prestressing steel.
Y1860S7= PrestressingSteel(steelName= "Y1860S7",fp01k= 0.85*1860e6, fmax= 1860e6)
Y1770= PrestressingSteel(steelName= "Y1770",fp01k= 1520e6, fmax= 1770e6, steelRelaxationClass= 2)
