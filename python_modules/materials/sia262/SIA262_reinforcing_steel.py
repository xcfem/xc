# -*- coding: utf-8 -*-
'''reinforcing steel material according to SIA 262 standard (Switzerland).'''

import math
import scipy.interpolate
from materials import reinforcingSteel

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"



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
