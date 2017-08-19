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
import scipy.interpolate
from materials import concrete_base

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

# Concrete according to Eurocode 2.


class EC2Concrete(concrete_base.Concrete):
    """Concrete model according to Eurocode 2

       :ivar typeAggregate: type of aggregate
 
                            - ``Q`` for quartzite aggregate 
                            - ``L`` for limestone aggregate
                            - ``S`` for sandstone aggregate
                            - ``B`` for basalt aggregate

    """
    typeAggregate='Q'

    def __init__(self,nmbConcrete, fck, gammaC):
        super(EC2Concrete,self).__init__(nmbConcrete,fck, gammaC)
    
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
        """Fctm: mean tensile strength [Pa][+] (table 3.1 EC2)
        """
        if self.fckMPa() <= 50:
            return 0.3*self.fckMPa()**(2.0/3.0)*1e6
        else:
            return 2.12*math.log(1+abs(self.getFcm())*1e-6/10)*1e6

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

#    def getEcmT(self):
#        """
#        EC2Ecmt: approximate value of the modulus of elasticity [Pa] at age 
#        t days, according to Eurocode2
#        """
#        return (self.getFcmT()/self.getFcm())**0.3*self.getEcm()




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
