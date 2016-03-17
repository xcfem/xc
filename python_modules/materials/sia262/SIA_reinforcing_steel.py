# -*- coding: utf-8 -*-

import math
from materials import reinforcingSteel


# Aciers SIA (tableau 5 SIA-262: propriétés de l'acier d'armature passive)
# Valeurs de calcul selon le tableau 9, chiffre 4.2 de SIA-262 
#  acier B500A: fsk=500MPa,, k=ftk/fsk>=1,05,, epsuk>=2,5%
#               fsd=435MPa,, ks=1,05,, epsud=2,0%
#  acier B500B: fsk=500MPa,, k=ftk/fsk>=1,08,, epsuk>=5,0%
#               fsd=435MPa,, ks=1,08,, epsud=4,5%
#Attributes:
#   nmbAcero: Name identifying the material.
#   fyk:      Characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   Partial factor for material.
#   k:        fmaxk/fyk ratio
B500A= reinforcingSteel.ReinforcingSteel(nmbAcero="B500A", fyk=500e6, emax=0.02,gammaS=500/435,k=1.05)
B500B= reinforcingSteel.ReinforcingSteel(nmbAcero="B500B", fyk=500e6, emax=0.045,gammaS=500/435,k=1.08)
