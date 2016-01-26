# -*- coding: utf-8 -*-
# home made test
# Testing hinge development in a cantilever.

import xc_base
import geom
import xc

from materials.perfiles_metalicos.arcelor import perfiles_ipe_arcelor as ipe
from materials import aceros_estructurales as steel

test= xc.ProblemaEF()
preprocessor=  test.getPreprocessor

S275JR= steel.S275JR
S275JR.gammaM= 1.05
epp= S275JR.getDesignElasticPerfectlyPlasticMaterial(preprocessor, "epp")
IPE200= ipe.IPEProfile(S275JR,'IPE_200')
reg= IPE200.discretization(preprocessor,"epp")
