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

import math
from materials import concrete_base

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
        if self.typeAggregate == 'L':
            return 0.9*EcmGPa*1e9
        elif self.typeAggregate == 'S':
            return 0.7*EcmGPa*1e9
        elif self.typeAggregate == 'B':
            return 1.2*EcmGPa*1e9
        else:
            return EcmGPa*1e9

    def getFctm(self):
        """Fctm: mean tensile strength [Pa][+] (table 3.1 EC2)."""
        if(self.fckMPa()<=50):
            retval= 0.3*math.pow(self.fckMPa(),2.0/3.0)*1e6
        else:
            retval= 2.12*math.log(1+abs(self.getFcm())*1e-6/10)*1e6
        return retval

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
        if self.fckMPa()<=50:
            return 3.5*(-1e-3)
        else:
            return (2.6+35*((90-self.fckMPa())/100)**4)*(-1e-3)

    def getShearStrengthReductionFactor(self, nationalAnnex= None):
        ''' Return the strength reduction factor for concrete cracked in shear
            according to expression 6.6N of EC2:2004.
        
        :param nationalAnnex: identifier of the national annex.
        '''
        return 0.6*(1-self.fckMPa()/250) # equation 6.6N
        
    def getMinShearReinfRatio(self, shearReinfSteel, nationalAnnex= None):
        ''' Return the minimum shear reinforcement ratio according
            to expression 9.5N (or the national annex substitution)
        
        :param shearReinfSteel: reinforcing steel material.
        :param nationalAnnex: identifier of the national annex.
        '''
        fyk= shearReinfSteel.fyk # characteristic value of the shear
                                 # reinforcement yield strength.
        if(nationalAnnex=='Spain'):
            retval= self.fctm()/7.5/fyk # Spanish national annex.
        else:
            retval= 0.08*math.sqrt(self.fckMPa())/(fyk/1e6)
        return retval

    def getAlphaCw(self, NEd, Ac, nationalAnnex= None):
        ''' Return the coefficient taking account of the state of the stress 
            in the compression chord according to expressions 6.11aN, 6.11bN
            and 6.11cN of EC2:2004.

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
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; excessive concrete stress: '+str(sigma_cp/1e6)+' MPa.')
            retval= 1e-6
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

    def __init__(self,nmbConcrete, fck, gammaC):
        super(EC2Concrete2021,self).__init__(nmbConcrete,fck, gammaC)

    # Autogenous shrinkage strain
    def getShrAlphabs(self):
        '''value of the alpha_bs coefficient accordint to table B.3 of 
           Eurocode2:2021 part 1-1.
        '''
        if self.cemType=='R':    #high early strength
            retval= 600
        elif self.cemType=='S':  #low early strength
            retval= 800
        else:                   # ordinary early strength
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
        '''
        rhEq= self.getShrRHeq()
        frac= RH/rhEq
        retval= -1.55
        if(RH<20):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; relative humidity: '+str(HR)+' out of range (20%<=HR<=100%).')
        elif(RH<=rhEq): # equation B.29a
            retval*= (1-frac**3)
        elif(RH<=100):
            retval*= (1-frac**2)
            if(retval==100): retval+= 25
        else: # RH>100 impossible.
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; relative humidity: '+str(HR)+' out of range (20%<=HR<=100%).')        
        return retval

    def getShrBetadstts(self, t, ts, h0):
        '''time development of drying shrinkage according to expression B31
           of clause B.6 of Eurocode2:2021 part 1-1.

        :param t: age of concrete in days at the moment considered
        :param ts: age of concrete in days at the beginning of drying shrinkage (or swelling). Normally this is at the end of curing
        :param h0: notional size of the member expressed in millimeters.
               - h0=``2*Ac/u``, where:
               - Ac= cross sectional area
               - u = perimeter of the member in contact with the atmosphere
        '''
        return math.pow((t-ts)/(0.035*((h0*1e3)**2)+(t-ts)),0.5)
    
    def getShrEpscdsfcm(self):
        '''notional drying shrinkage coefficient according to expression
           according to expression B28 of clause B.6 of Eurocode 2:2021
           part 1-1.
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

#
#   steelName: Name identifying the material.
#   fyk:      characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   partial factor for material.
#   k:        fmaxk/fyk ratio
S400A= concrete_base.ReinforcingSteel(steelName="S400A", fyk=400e6, emax=0.025,gammaS=1.15,k=1.05)
S400B= concrete_base.ReinforcingSteel(steelName="S400B", fyk=400e6, emax=0.05,gammaS=1.15,k=1.08)
S400C= concrete_base.ReinforcingSteel(steelName="S400C", fyk=400e6, emax=0.075,gammaS=1.15,k=1.15)

S450A= concrete_base.ReinforcingSteel(steelName="S450A", fyk=450e6, emax=0.025,gammaS=1.15,k=1.05)
S450B= concrete_base.ReinforcingSteel(steelName="S450B", fyk=450e6, emax=0.05,gammaS=1.15,k=1.08)
S450C= concrete_base.ReinforcingSteel(steelName="S450C", fyk=450e6, emax=0.075,gammaS=1.15,k=1.15)

S500A= concrete_base.ReinforcingSteel(steelName="S500A", fyk=500e6, emax=0.025,gammaS=1.15,k=1.05)
S500B= concrete_base.ReinforcingSteel(steelName="S500B", fyk=500e6, emax=0.05,gammaS=1.15,k=1.08)
S500C= concrete_base.ReinforcingSteel(steelName="S500C", fyk=500e6, emax=0.075,gammaS=1.15,k=1.15)

S550A= concrete_base.ReinforcingSteel(steelName="S550A", fyk=550e6, emax=0.025,gammaS=1.15,k=1.05)
S550B= concrete_base.ReinforcingSteel(steelName="S550B", fyk=550e6, emax=0.05,gammaS=1.15,k=1.08)
S550C= concrete_base.ReinforcingSteel(steelName="S550C", fyk=550e6, emax=0.075,gammaS=1.15,k=1.15)

S600A= concrete_base.ReinforcingSteel(steelName="S600A", fyk=600e6, emax=0.025,gammaS=1.15,k=1.05)
S600B= concrete_base.ReinforcingSteel(steelName="S600B", fyk=600e6, emax=0.05,gammaS=1.15,k=1.08)
S600C= concrete_base.ReinforcingSteel(steelName="S600C", fyk=600e6, emax=0.075,gammaS=1.15,k=1.15)

steelOfName={"S400A":S400A,"S400B":S400B,"S400C":S400C,"S450A":S450A,"S450B":S450B,"S450C":S450C,"S500A":S500A,"S500B":S500B,"S500C":S500C,"S550A":S550A,"S550B":S550B,"S550C":S550C,"S600A":S600A,"S600B":S600B,"S600C":S600C}

# rebars as defined in Eurocode 2.
rebarsEC2={}

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
