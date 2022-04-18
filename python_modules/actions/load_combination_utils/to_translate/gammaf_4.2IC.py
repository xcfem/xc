# -*- coding: utf-8 -*-
# Coeficientes de ponderación según 4.2IC para acciones permanentes en situaciones de CONSTRUCCIÓN.

import loadCombinations

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

# ver tabla 1.1 de la colección de pequeñas obras de paso 4.2IC
partial_safety_factors["permanentes"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.6,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))

# Coeficientes de ponderación para acciones permanentes
# de valor no constante debidas a acciones provocadas por
# empujes o movimientos del terreno.
partial_safety_factors["permanentes_nc_terr"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,0.75*1.6,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))

partial_safety_factors["variables"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.6,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
