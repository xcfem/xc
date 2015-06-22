# -*- coding: utf-8 -*-

import math
from materials import reinforcingSteel

# Deformación máxima (0.08 para B400S y 0.05 para B500S)
B400S= reinforcingSteel.ReinforcingSteel("B400S", 400e6, 0.08,1.15)
B500S= reinforcingSteel.ReinforcingSteel("B500S", 500e6, 0.05,1.15)

