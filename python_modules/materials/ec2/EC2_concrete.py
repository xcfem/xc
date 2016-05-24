# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

# Concrete according to Eurocode 2.

import math
import scipy.interpolate
from materials import concreteBase

class EC2Concrete(concreteBase.Concrete):
    """Concrete model according to Eurocode 2
    Attributes:
        typeAggregate:type of aggregate=
            'Q' for quartzite aggregate 
            'L' for limestone aggregate
            'S' for sandstone aggregate
            'B' for basalt aggregate

    """
    typeAggregate='Q'

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
