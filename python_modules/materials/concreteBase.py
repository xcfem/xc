# -*- coding: utf-8 -*-
# Concrete according to EC2 and EHE.

from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com"


import math
import scipy.interpolate

from materials import typical_materials
from materials import materialGraphics
import matplotlib.pyplot as plt

class Concrete(object):
    """ Concrete model according to Eurocode 2 - Base class.
    Attributes:
        nmbMaterial: name of material.
        nmbDiagK: name of the characteristic diagram
        matTagK:  tag of the uni-axial material in the characteristic diagram
        nmbDiagD: name of the design diagram
        matTagD:  tag of the uni-axial material in the design diagram
        fck:       characteristic (5%) cylinder strength of the concrete [Pa]
        gmmC:      partial safety factor for concrete 
        nuc:       Poisson coefficient
        cemType:   type of cement:
                     = 'R'  for cement of strength Classes CEM 42,5 R, CEM 52,5 Nand CEM 52,5 R (Class R) 
                            [cementos de alta resistencia en EHE]
                     = 'N'  for cement of strength Classes CEM 32,5 R, CEM 42,5 N (Class N) 
                            [cementos normales en EHE]
                     = 'S'  for cement of strength Classes CEM 32,5 N (Class S) 
                            [cementos de endurecimiento lento en EHE]
        alfacc:    factor which takes account of the fatigue in the concrete when
                   it is subjected to high levels of compression stress due to long
                   duration loads. Normally alfacc=1 (default value)
    """
    nmbMaterial= "nil" #** Name of material.
    nmbDiagK= "dgK"+nmbMaterial #** Name of characteristic diagram.
    matTagK= -1 #** Tag of uni-axial material in the characteristic diagram.
    nmbDiagD= "dgD"+nmbMaterial #** Name of the design diagram.
    matTagD= -1 #** Tag of uni-axial material in the design diagram.
    fck=-25e6 #** characteristic (5%) cylinder strength of the concrete [Pa]
    gmmC= 1.5 #** Partial safety factor for concrete 
    nuc= 0.2 #** Poisson coefficient
    cemType='N'     #type of cement:
             #    = 0,20 for cement of strength Classes CEM 42,5 R, CEM 52,5 Nand CEM 52,5 R (Class R) 
             #           [cementos de alta resistencia en EHE]
             #    = 0,25 for cement of strength Classes CEM 32,5 R, CEM 42,5 N (Class N) 
             #           [cementos normales en EHE]
             #    = 0,38 for cement of strength Classes CEM 32,5 N (Class S) 
             #           [cementos de endurecimiento lento en EHE]
    alfacc=1

    # Definition of «derived» properties of the material.
    def __init__(self,nmbConcrete, fck, gammaC):
        self.setupName(nmbConcrete)
        self.fck= fck #** characteristic (5%) cylinder strength of the concrete [Pa]
        self.gmmC= gammaC #** Partial safety factor for concrete 
        self.concrDiagramK= None # Characteristic stress-strain diagram.
        self.concrDiagramD= None # Design stress-strain diagram.

    def setupName(self,nmbConcrete):
        self.nmbMaterial= nmbConcrete #** Name of material
        self.nmbDiagK= "dgK"+self.nmbMaterial #** Name of characteristic diagram.
        self.matTagK= -1 #** Tag of uni-axial material in the characteristic diagram.
        self.nmbDiagD= "dgD"+self.nmbMaterial #** Name of the design diagram.
        self.matTagD= -1 #** Tag of uni-axial material in the characteristic diagram. 

    def fcd(self):
        '''design strength of the concrete [Pa][-]'''
        return self.alfacc*self.fck/self.gmmC 

    def fmaxK(self):
        '''maximum characteristic strength of the concrete [Pa][-]'''
        #in the previous version fmaxK was calculated as the product of 0.85 by fck
#        return self.alfacc*self.fck
        return self.fck

    def fmaxD(self):
        return self.fcd()

    def fckMPa(self):
        """characteristic (5%) cylinder strength of the concrete in MPa (absolute value)""" 
        return abs(self.fck)*1e-6

    def fctm(self):
        """mean tensile strength [Pa][+] (table 3.1 EC2)"""
        return self.getFctm() 

    def fctk(self):
        """characteristic tensile strength [Pa] (5% fractile)"""
        return self.getFctk005()

    def fctd(self):
        """Design mean tensile strength [Pa][+]."""
        return abs(self.fctk())/self.gmmC 

    def epsilon0(self):
        """ strain at peak stress at parabola-rectangle diagram"""
        return self.getEpsc2()

    def epsilonU(self):
        """ nominal ultimate strain at parabola-rectangle diagram"""
        return self.getEpscu2()

    def expnPR(self):
        """ exponent n for the parabola-rectangle diagram """
        return self.getExpN()

    def Ecm(self):
        """Módulo de deformación longitudinal secante a 28 días expresado
        en [Pa] [+] 
        """
        return self.getEcm()

    def Gcm(self):
        """ Shear elactic modulus [Pa][+] """
        return self.getEcm()/(2*(1+self.nuc))  #**

    def getFcm(self):
        """Fcm: mean compressive strength [Pa][-] at 28 days 
        (table 3.1 EC2, art. 39.6 EHE-08)
        """
        return (self.fckMPa()+8)*(-1e6)
 
    def getFctk005(self,t=28):
        """
        Fctk005: tensile strength [Pa][+] 5% fractile (table 3.1 EC2)
        Attributes:
        t:      age of the concrete in days
        """
        return 0.7*self.getFctm()
 
    def getFctk095(self,t=28):
        """
        Fctk095: tensile strength [Pa][+] 95% fractile (table 3.1 EC2)
        Attributes:
        t:      age of the concrete in days
        """
        return 1.3*self.getFctm()

    def defDiagK(self,preprocessor):
        ''' Defines a Concrete01 uniaxial material to represent the characteristic stress-strain diagram.'''
        self.concrDiagramK= typical_materials.defConcrete01(preprocessor,self.nmbDiagK,self.epsilon0(),self.fmaxK(),self.fmaxK(),self.epsilonU())
        self.matTagK= self.concrDiagramK.tag
        return self.matTagK

    def defDiagD(self,preprocessor):
        ''' Defines a Concrete01 uniaxial material to represent the design stress-strain diagram..'''
        self.concrDiagramD= typical_materials.defConcrete01(preprocessor,self.nmbDiagD,self.epsilon0(),self.fmaxD(),self.fmaxD(),self.epsilonU())
        self.matTagD= self.concrDiagramD.tag
        return self.matTagD

    def getDiagD(self,preprocessor):
        return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagD)

    def getDiagK(self,preprocessor):
        return preprocessor.getMaterialLoader.getMaterial(self.nmbDiagK)

    def sigmaPR(self,eps):
        """ stress as function of strain according to parabola-rectangle diagram"""
        return self.sigmac(eps)

#Time-dependent properties of concrete according to Eurocode 2
    def getBetaCC(self,t=28):
        """
        beta_cc: coefficient (sect. 3.1.2 EC2, art. 31.3 EHE-08)
        Attributes:
        t:      age of the concrete in days
        """
        if self.cemType=='R':    #high-strength cement
            s=0.20
        elif self.cemType=='S':  #slow cement
            s=0.38
        else:                   #normal cement
            s=0.25
        return math.exp(s*(1-math.sqrt(28/t)))

    def getFcmT(self,t=28):
        """FcmT: mean concrete compressive strength [Pa][-] at an age of t days 
        (sect. 3.1.2 EC2, art. 31.3 EHE-08)
        Attributes:
        t:      age of the concrete in days
        """
        return self.getFcm()*self.getBetaCC(t)

    def getFckt(self,t=28):
        """Fckt: concrete compressive strength [Pa][-] at time t (for stages) 
           (sect. 3.1.2 EC2)
        Attributes:
        t:      age of the concrete in days
        """
        if t >= 28:
            return self.fckMPa()*(-1e6)
        elif t > 3:
            return (abs(self.getFcmT(t))*(1e-6)-8)*(-1e6)
        else:
            return "t must be > 3 days"

#Shrinkage
#   Basic drying shrinkage
    def getShrEpscd0(self,RH):
        '''Basic drying shrinkage strain [-]
        for the calculation of the drying shrinkage strain
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            RH:  ambient relative humidity(%)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        fcm0MPa=10       
        epscd0=0.85*((220+110*self.getShrAlfads1())*math.exp(-self.getShrAlfads2()*fcmMPa/fcm0MPa))*1e-6*self.getShrBetaRH(RH)
        return epscd0*(-1)


    def getShrBetaRH(self,RH):
        '''Coefficient for the calculation of the
        basic drying shrinkage strain 
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            RH:  ambient relative humidity(%)
        '''
        if RH<99:
            betaRH=1.55*(1-(RH/100)**3)
        else:
            betaRH=0.25
        return betaRH

    def getShrAlfads1(self):
        '''Coefficient for the calculation of the
        basic drying shrinkage strain 
        (Annex B Eurocode 2 part 1-1)
        '''
        if self.cemType=='R':    #high-strength cement
            alfads1=6
        elif self.cemType=='S':  #slow cement
            alfads1=3
        else:                   #normal cement
            alfads1=4
        return alfads1

    def getShrAlfads2(self):
        '''Coefficient for the calculation of the
        basic drying shrinkage strain 
        (Annex B Eurocode 2 part 1-1)
        '''
        if self.cemType=='R':    #high-strength cement
            alfads2=0.11
        elif self.cemType=='S':  #slow cement
            alfads2=0.13
        else:                    #normal cement
            alfads2=0.12
        return alfads2

    def getShrEpscd(self,t,ts,RH,h0mm):
        '''Drying shrinkage strain[-] (art. 3.1.4 Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
            ts:    age of concrete in days at the beginning of drying shrinkage (or swelling)
                   Normally this is at the end of curing
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        #epscd: drying shrinkage strain
        epscd=self.getShrBetadstts(t,ts,h0mm)*self.getShrKh(h0mm)*self.getShrEpscd0(RH)
        return epscd

    def getShrKh(self,h0mm):
        '''coefficient depending on the notional size h0mm
        for the calculation of the drying shrinkage strain
        (table 3.3 Eurocode 2 part 1-1)
        Attributes:
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        x=(0,100,200,300,500,1e10)
        y=(1.0,1.0,0.85,0.75,0.70,0.70)
        f=scipy.interpolate.interp1d(x, y)
        kh=f(h0mm)
        return kh

    def getShrBetadstts(self,t,ts,h0mm):
        '''coefficient for the calculation of the drying shrinkage strain
        (table 3.3 Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
            ts:    age of concrete in days at the beginning of drying shrinkage (or swelling)
                   Normally this is at the end of curing
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        betadstts=(t-ts)/(t-ts+0.04*(h0mm)**(3.0/2.0))
        return betadstts

#   Autogenous shrinkage strain
    def getShrEpsca(self,t):
        '''Autogenous shrinkage strain 
        (art. 3.1.4 Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
        '''
        epsca=self.getShrEpscainf(t)*self.getShrBetaast(t)
        return epsca

    def getShrEpscainf(self,t):
        '''coefficient for calculating the autogenous shrinkage strain [-]
        (art. 3.1.4 Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
        '''
        epscainf=2.5*(self.fckMPa()-10)*1e-6
        return epscainf*(-1)

    def getShrBetaast(self,t):
        '''coefficient for calculating the autogenous shrinkage strain
        (art. 3.1.4 Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
        '''
        betaast=1-math.exp(-0.2*t**0.5)
        return betaast

#Total shrinkage
    def getShrEpscs(self,t,ts,RH,h0mm):
        '''Total shrinkage strain = 
        autogenous + drying shrinkages
        Attributes:
            t:     age of concrete in days at the moment considered
            ts:    age of concrete in days at the beginning of drying shrinkage (or swelling)
                   Normally this is at the end of curing
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        epscs=self.getShrEpscd(t,ts,RH,h0mm)+self.getShrEpsca(t)
        return epscs

#Creep
    def getCreepFitt0(self,t,t0,RH,h0mm):
        '''Creep coefficient  
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
            t0:    age of concrete in days at loading
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        fitt0=self.getCreepFi0(t0,RH,h0mm)*self.getCreepBetactt0(t,t0,RH,h0mm)
        return fitt0

    def getCreepAlfa1(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa1=(35/fcmMPa)**0.7
        return alfa1

    def getCreepAlfa2(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa2=(35/fcmMPa)**0.2
        return alfa2

    def getCreepAlfa3(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa3=(35/fcmMPa)**0.5
        return alfa3

    def getCreepFiRH(self,RH,h0mm):
        '''factor to allow for the effect of relative humidity 
        on the notional creep coefficient
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        fcmMPa=abs(self.getFcm())/1e6
        if fcmMPa <= 35:
            fiRH=1+(1-RH/100)/0.1/h0mm**(1/3.0)
        else:
            fiRH=(1+(1-RH/100)/0.1/h0mm**(1/3.0)*self.getCreepAlfa1())*self.getCreepAlfa2()
        return fiRH

    def getCreepBetafcm(self):
        '''factor to allow for the effect of concrete strength
        on the notational creep coefficient
        (Annex B Eurocode 2 part 1-1)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        betafcm=16.8/fcmMPa**(1/2.0)
        return betafcm

    def getCreepBetat0(self,t0):
        '''factor to allow for the effect of concrete age at loading
        on the notational creep coefficient
       (Annex B Eurocode 2 part 1-1)
        Attributes:
            t0:    age of concrete in days at loading
        '''
        betat0=1/(0.1+t0**0.20)
        return betat0

    def getCreepFi0(self,t0,RH,h0mm):
        '''notational creep coefficient for the calculation 
        of the creep coefficient
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            t0:    age of concrete in days at loading
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        fi0=self.getCreepFiRH(RH,h0mm)*self.getCreepBetafcm()*self.getCreepBetat0(t0)
        return fi0

    def getCreepBetactt0(self,t,t0,RH,h0mm):
        '''coefficient to describe the development of creep 
        with time after loading, used to calculate the creep coefficient
        (Annex B Eurocode 2 part 1-1)
        Attributes:
            t:     age of concrete in days at the moment considered
            t0:    age of concrete in days at loading
            RH:    ambient relative humidity(%)
            h0mm:  notional size of the member in mm
                   h0mm=2*Ac/u, where:
                        Ac= cross sectional area
                        u = perimeter of the member in contact with the atmosphere
        '''
        fcmMPa=abs(self.getFcm())/1e6
        if fcmMPa <= 35:
            betaH=min(1.5*(1+(0.012*RH)**18)*h0mm+250,1500)
        else:
            betaH=min(1.5*(1+(0.012*RH)**18)*h0mm+250*self.getCreepAlfa3(),1500*self.getCreepAlfa3())
        betactt0=((t-t0)/(betaH+t-t0))**0.3
        return betactt0

#Parabola-rectangle diagrams for concrete under compression 
    def sigmac(self,eps):
        '''Stress [Pa][-] from parabola-rectangle diagram
        Atributtes:
            eps: strain [-]
        '''
        if(eps<0):
            if(eps>self.getEpsc2()):
                return self.fcd()*(1-(1-eps/self.getEpsc2())**self.getExpN())
            else:
                return self.fcd()
        else:
            return 0.0 

    def tangc(self,eps):
        ''' Tangent of the parabola-rectangle diagram
        Atributtes:
            eps: strain [-]
        '''
        if(eps<0):
          if(eps>self.getEpsc2()):
            return self.fcd()*self.getExpN()*(1-eps/self.getEpsc2())**(self.getExpN()-1)*(1/self.getEpsc2())
          else:
            return 0.0
        else:
          return 0.0 

    def plotDesignStressStrainDiagram(self):
        retval= materialGraphics.UniaxialMaterialDiagramGraphic(epsMin=self.epsilonU(),epsMax=0,title=self.nmbMaterial + ' design stress-strain diagram')
        retval.setupGraphic(plt,self.concrDiagramD)
        fileName= self.nmbMaterial+'_design_stress_strain_diagram'
        retval.savefig(plt,fileName+'.jpeg')
        retval.savefig(plt,fileName+'.eps')
        return retval


def defDiagKConcrete(preprocessor, concreteRecord):
  print 'defDiagKConcrete deprecated; use concreteRecord.defDiagK(preproccesor)'
  return concreteRecord.defDiagK(preprocessor);

def defDiagDConcrete(preprocessor, concreteRecord):
  print 'defDiagDConcrete deprecated; use concreteRecord.defDiagD(preproccesor)'
  return concreteRecord.defDiagD(preprocessor)






# Calculates the differece between the stresses obtained from de Concrete01 uniaxial material (getStress() ) and the theoretical law defined in Python (see sigmac()).
def testDiagDHormigon(preprocessor, concreteRecord):
    tag= concreteRecord.defDiagD(preprocessor)
    diagConcrete= preprocessor.getMaterialLoader.getMaterial(concreteRecord.nmbDiagD)
    incr= concreteRecord.epsilonU()/20
    errMax= 0.0
    e=  -0.1e-8
    while(e>=concreteRecord.epsilonU()):
      diagConcrete.setTrialStrain(e,0.0)
      diagConcrete.commitState()
#      sg= sigmaDConcrete(e,concreteRecord)
      sg=concreteRecord.sigmac(e)
      stress= diagConcrete.getStress()
      err= abs((sg-stress)/sg)
      errMax= max(err,errMax)
      e=(e+incr)
    return errMax

# Calculates the differece between the values of the tangent obtained from de Concrete01 uniaxial material (getTangent() ) and the theoretical law defined in Python (see tangc()).
def testTangDHormigon(preprocessor, concreteRecord):
    tag= concreteRecord.defDiagD(preprocessor)
    diagConcrete= preprocessor.getMaterialLoader.getMaterial(concreteRecord.nmbDiagD)
    incr= concreteRecord.epsilonU()/20
    errMax= 0.0
    e=-0.1e-8
    while(e>=concreteRecord.epsilonU()):
      diagConcrete.setTrialStrain(e,0.0)
      diagConcrete.commitState()
#      tg= concreteRecord.tangDConcrete(e)
      tg=concreteRecord.tangc(e)
      tgDiag= diagConcrete.getTangent()
      if(tg!=0.0):
        err= abs((tg-tgDiag)/tg)
      else:
        err= abs(tg-tgDiag)
      errMax= max(err,errMax)
      e=(e+incr)
    return errMax





#SIA262 concretes.
#same denomination of concretes that those in EC2



