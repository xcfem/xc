# -*- coding: utf-8 -*-
'''Reinforce concrete materials according to SIA 262 standard (Switzerland).'''

import math
import scipy.interpolate
from materials.ec2 import EC2_materials as ec2
from materials import reinforcingSteel

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"


class ConcreteSIA262(ec2.EC2Concrete):
  def nufc(self):
    return min(math.pow(30e6/-self.fck,1/3.0),1.0)
  def fctd(self,t,favourable):
    '''SIA 262 sections 4.4.1.3 et 4.4.1.4'''
    kt= 1.0/(1.0+0.5*t) #(99)
    retval= 0.0
    if favourable:
      retval= kt*self.fctm() #(98)
    else:
      retval= kt*self.getFctk095() #(99)
    return retval
  def fctk0_05(self):
    '''SIA 262 sections 3.1.2.2.5 expr (7)'''
    return 0.7*self.getFctm()
  def fctk0_95(self):
    '''SIA 262 sections 3.1.2.2.5 expr (8)'''
    return 1.3*self.getFctm()
  def taucd(self):
    return 0.3*math.sqrt(-self.fck/1e6)*1e6/self.gmmC
  def kDmax(self):
    return 48/(self.Dmax*1e3+16)

c12_15= ConcreteSIA262("C12-15",-12e6,1.5)
c16_20= ConcreteSIA262("C16-20",-16e6,1.5)
c20_25= ConcreteSIA262("C20-25",-20e6,1.5)
c25_30= ConcreteSIA262("C25-30",-25e6,1.5)
c30_37= ConcreteSIA262("C30-37",-30e6,1.5)
c35_45= ConcreteSIA262("C35-45",-35e6,1.5)
c40_50= ConcreteSIA262("C40-50",-40e6,1.5)
c45_55= ConcreteSIA262("C45-55",-45e6,1.5)
c50_60= ConcreteSIA262("C50-60",-50e6,1.5)

def reductionFactorKT(t):
  return 1/(1+0.5*t)

# Reinforcing steel material.


# Aciers SIA (tableau 5 SIA-262: propriétés de l'acier d'armature passive)
# Valeurs de calcul selon le tableau 9, chiffre 4.2 de SIA-262 
#  acier B500A: fsk=500MPa,, k=ftk/fsk>=1,05,, epsuk>=2,5%
#               fsd=435MPa,, ks=1,05,, epsud=2,0%
#  acier B500B: fsk=500MPa,, k=ftk/fsk>=1,08,, epsuk>=5,0%
#               fsd=435MPa,, ks=1,08,, epsud=4,5%
#
#   nmbAcero: Name identifying the material.
#   fyk:      Characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   Partial factor for material.
#   k:        fmaxk/fyk ratio
B500A= reinforcingSteel.ReinforcingSteel(nmbAcero="B500A", fyk=500e6, emax=0.02, gammaS=500/435, k=1.05)
B500B= reinforcingSteel.ReinforcingSteel(nmbAcero="B500B", fyk=500e6, emax=0.045, gammaS=500/435, k=1.08)
B500C= reinforcingSteel.ReinforcingSteel(nmbAcero="B500C", fyk=500e6, emax=0.065, gammaS=500/435, k=1.15)
B700B= reinforcingSteel.ReinforcingSteel(nmbAcero="B700B", fyk=700e6, emax=0.045, gammaS=500/435, k=1.08)

SpecialII1956SIA161= reinforcingSteel.ReinforcingSteel(nmbAcero="SpecialII", fyk=300e6, emax=0.045,gammaS=1.15,k=1.05)

x= [50e-3,100e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,435e6,435e6,435e6,435e6]
courbeA= scipy.interpolate.interp1d(x,y)

x= [50e-3,100e-3,130e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,435e6,400e6,340e6,280e6,260e6]
courbeB= scipy.interpolate.interp1d(x,y)

x= [50e-3,55e-3,100e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,290e6,230e6,190e6,160e6,140e6]
courbeC= scipy.interpolate.interp1d(x,y)


def limitationContraintes(exigence,s):
  if(exigence=="A"):
    return courbeA(s)
  elif (exigence=="B"):
    return courbeB(s)
  elif (exigence=="C"):
    return courbeC(s)
  else:
    print "Value for exigence: '",exigence,"' unknown."

diametres= [6e-3,8e-3,10e-3,12e-3,14e-3,16e-3,18e-3,20e-3,22e-3,26e-3,30e-3,34e-3,40e-3]

section_barres_courantes={}

for d in diametres:
  section_barres_courantes[d]= math.pi*(d/2.0)**2

def numBars(AsNec):
  retval= []
  for d in diametres:
    a= math.pi*(d/2.0)**2
    n= math.ceil(AsNec/a)
    retval.append((d,n))
  return retval
