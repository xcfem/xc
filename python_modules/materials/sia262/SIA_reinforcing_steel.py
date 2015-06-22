# -*- coding: utf-8 -*-

import math
from materials import reinforcingSteel


# Aciers SIA (tableau 5 SIA-262: propriétés de l'acier d'armature passive)
# Valeurs de calcul selon le tableau 9, chiffre 4.2 de SIA-262 
#  acier B500A: fsk=500MPa,, k=ftk/fsk>=1,05,, epsuk>=2,5%
#               fsd=435MPa,, ks=1,05,, epsud=2,0%
#  acier B500B: fsk=500MPa,, k=ftk/fsk>=1,08,, epsuk>=5,0%
#               fsd=435MPa,, ks=1,08,, epsud=4,5%
B500A= reinforcingSteel.ReinforcingSteel("B500A", 500e6, 0.02,500/435,1.05)
B500B= reinforcingSteel.ReinforcingSteel("B500B", 500e6, 0.045,500/435,1.08)
