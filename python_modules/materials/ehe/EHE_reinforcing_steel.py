# -*- coding: utf-8 -*-

'''EHE_reinforcing_steel.py: reinforcing steel as defined in EHE (spanish RC standard).'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

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
