# -*- coding: utf-8 -*-
# home made test
# Testing hinge development in a cantilever.

import xc_base
import geom
import xc

from materials.perfiles_metalicos.arcelor import perfiles_ipe_arcelor as ipe

S275JR.gammaM= gammaM0 
IPE200= steelProfile.SteelProfile(S275JR,"HE_400_B",HE_profiles.perfilesHE)
profil= IPE200.defSeccShElastica3d(preprocessor)
