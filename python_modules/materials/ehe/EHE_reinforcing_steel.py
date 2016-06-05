# -*- coding: utf-8 -*-

'''EHE_reinforcing_steel.py: reinforcing steel as defined in EHE (spanish RC standard).'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
from materials import reinforcingSteel

# Deformación máxima (0.08 para B400S y 0.05 para B500S)
#
#   nmbAcero: Name identifying the material.
#   fyk:      Characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   Partial factor for material.
#   k:        fmaxk/fyk ratio
B400S= reinforcingSteel.ReinforcingSteel(nmbAcero="B400S", fyk=400e6, emax=0.08,gammaS=1.15)
B500S= reinforcingSteel.ReinforcingSteel(nmbAcero="B500S", fyk=500e6, emax=0.05,gammaS=1.15)

