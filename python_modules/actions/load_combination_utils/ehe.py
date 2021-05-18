# -*- coding: utf-8 -*-

import xc_base
import loadCombinations

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#Coeficientes de ponderación para acciones permanentes de valor no constante, control de ejecución normal.
partial_safety_factors["permanentes_nc_ctr_normal"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.5,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables, control de ejecución normal.
partial_safety_factors["variables_ctr_normal"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones sísmicas, control de ejecución normal.
partial_safety_factors["sismicas_ctr_normal"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Coeficientes de ponderación para acciones accidentales, control de ejecución normal.
partial_safety_factors["accidentales_ctr_normal"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Coeficientes de ponderación para acciones permanentes, control de ejecución intenso.
partial_safety_factors["permanentes_ctr_intenso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))

#Coeficientes de ponderación para acciones permanentes de valor no constante, control de ejecución intenso.
partial_safety_factors["permanentes_nc_ctr_intenso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.5,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables, control de ejecución intenso.
partial_safety_factors["variables_ctr_intenso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones sísmicas, control de ejecución intenso.
partial_safety_factors["sismicas_ctr_intenso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Coeficientes de ponderación para acciones accidentales, control de ejecución intenso.
partial_safety_factors["accidentales_ctr_intenso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))

combination_factors= factors.getCombinationFactors()

combination_factors.insert("permanents",loadCombinations.CombinationFactors(1,1,1))
combination_factors.insert("snow",loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert("wind",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("thermal",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("sismo",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("accidental",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("azotea_solo_conserv",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("vivienda",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("dormitorios_hotel",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("pasillos_escaleras_hotel",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("salones_hotel",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("aulas_escuela",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("pasillos_escaleras_escuela",loadCombinations.CombinationFactors(0.7,0.7,0.6))
#Coeficientes de ponderación para acciones permanentes, control de ejecución normal.
partial_safety_factors["permanentes_ctr_normal"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))


intenseControlCombGenerator= loadCombinations.LoadCombGenerator()
actionsAndFactors= intenseControlCombGenerator.actionWeighting.create("EHEIntenso",factors)
