# -*- coding: utf-8 -*-
'''Reinforcing steel as defined in ACI 318 VERIFICATION TEST REQUIRED!!!.'''
from materials import reinforcingSteel

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

A706G60= reinforcingSteel.ReinforcingSteel(nmbAcero="A706G60", fyk=420e6, emax=0.08,gammaS=1.15)


#Bar areas in square meters.

num3Area= 71e-6
num4Area= 129e-6
num5Area= 200e-6
num6Area= 284e-6
num7Area= 387e-6
num8Area= 509e-6
num9Area= 645e-6
num10Area= 819e-6
num11Area= 1006e-6
num14Area= 1452e-6
num18Area= 2581e-6
num18JArea= 2678e-6
