# -*- coding: utf-8 -*-

import xc_base
import loadCombinations

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#Coeficientes de ponderación para acciones permanentes.
partial_safety_factors["permanentes_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,1.6,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante.
partial_safety_factors["permanentes_nc_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,0.75*1.6,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables.
partial_safety_factors["variables_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.6,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))


