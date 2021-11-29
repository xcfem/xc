# -*- coding: utf-8 -*-

from __future__ import print_function
import xc_base
import loadCombinations


g_els= loadCombinations.GammaFELS(1,2)

g_elu= loadCombinations.GammaFELU(1,2,3,4)

g= loadCombinations.GammaF(g_elu,g_els)
print("favorable= ", g.getGammaFELU.favorable, " desfavorable= ", g.getGammaFELU.desfavorable, "favorable accidental= ", g.getGammaFELU.favorableAccidental, " desfavorable accidental= ", g.getGammaFELU.desfavorableAccidental)
print("favorable= ", g.getGammaFELS.favorable, " desfavorable= ", g.getGammaFELS.desfavorable)

g_els.setProp("nuevaPropiedad","Hello")
g_elu.nuevaPropiedad= "No sé"
g_elu.setProp("nuevaPropiedad","Bye")

print("checking1: ", g_els.getProp("nuevaPropiedad"))
print("checking2: ", g_elu.getProp("nuevaPropiedad"))
print("checking3: ", g_elu.nuevaPropiedad)
