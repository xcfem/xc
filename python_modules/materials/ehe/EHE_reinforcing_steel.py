# -*- coding: utf-8 -*-

'''EHE_reinforcing_steel.py: reinforcing steel as defined in EHE (spanish RC standard).'''

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials import reinforcingSteel

# Deformación máxima (0.08 para B400S y 0.05 para B500S)
B400S= reinforcingSteel.ReinforcingSteel("B400S", 400e6, 0.08,1.15)
B500S= reinforcingSteel.ReinforcingSteel("B500S", 500e6, 0.05,1.15)

