# -*- coding: utf-8 -*-

import loadCombinations

factors= loadCombinations.Factors()

#Coef.FULS("favorable","desfavorable","favorable accidental","desfavorable accidental")


#Partial safety factors EC0
# -Persistent and transient situations: table AN.9 (A2.4(B) (STR/GEO) (Set B) )
# -Accidental situations: table AN.10 (table A2.5).
partial_safety_factors= factors.getPartialSafetyFactors()

#Partial safety factors for permanent actions
partial_safety_factors["permanent"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))


# Combination factors for road bridges (table AN.5 (table A2.1) )
combination_factors= factors.getCombinationFactors()
# 
combination_factors.insert("permanent",loadCombinations.CombinationFactors(1,1,1))

controlCombGenerator= loadCombinations.LoadCombGenerator()
actionsAndFactors= controlCombGenerator.actionWeighting.create("EC0_ES",factors)
