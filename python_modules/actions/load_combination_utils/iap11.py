# -*- coding: utf-8 -*-

import xc_base
import loadCombinations

factors= loadCombinations.Factors()

#Coef. FULS("favorable","desfavorable","favorable accidental","desfavorable accidental")
#Coeficientes de ponderación para acciones permanentes de valor constante (G)
#(válido para peso propio y carga muerta)

#Partial safety factors IAP11 (tabla 6.1-a)
partial_safety_factors= factors.getPartialSafetyFactors()

partial_safety_factors["permanentes"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))

#Coef. de ponderación para acciones permanentes de valor no constante (G*)
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P1 (armaduras postesas).
partial_safety_factors["permanentes_nc_Pret1Post"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1,1,1),loadCombinations.SLSPartialSafetyFactors(0.9,1.1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P1 (armaduras pretesas).
partial_safety_factors["permanentes_nc_Pret1Pret"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1,1,1),loadCombinations.SLSPartialSafetyFactors(0.95,1.05))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P2.
partial_safety_factors["permanentes_nc_Pret2"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a otras presolicitaciones distintas del pretensado.
partial_safety_factors["permanentes_nc_OtrasPresol"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.00,1.00,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a efectos reológicos (retracción y fluencia).
partial_safety_factors["permanentes_nc_Reol"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a acciones provocadas por empujes o movimientos del terreno.
partial_safety_factors["permanentes_nc_Terr"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a asientos en cálculo elástico
partial_safety_factors["permanentes_nc_AsientElast"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.2,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a asientos en cálculo elasto-plástico
partial_safety_factors["permanentes_nc_AsientPlast"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a rozamiento de apoyos deslizantes
partial_safety_factors["permanentes_nc_RozApoyos"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))


#Coeficientes de ponderación para acciones variables (Q).
#Sobrecarga de  uso
partial_safety_factors["variables_SCuso"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Sobrecarga de uso en terraplenes
partial_safety_factors["variables_SCusoTerrapl"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Acciones climáticas
partial_safety_factors["variables_climatica"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Empuje hidrostático
partial_safety_factors["variables_empHidrost"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Empuje hidrodinámico
partial_safety_factors["variables_empHidrodin"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Sobrecargas de construcción
partial_safety_factors["variables_SCconstruccion"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))


#Coeficientes de ponderación para acciones accidentales.
partial_safety_factors["accidentales"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))

#Coeficientes de ponderación para acciones sísmicas.
partial_safety_factors["sismicas"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1), loadCombinations.SLSPartialSafetyFactors(0,0))

#Factores de simultaneidad (tabla 6.1-a)
combination_factors= factors.getCombinationFactors()
# SC uso
#cargas verticales
combination_factors.insert("permanentes",loadCombinations.CombinationFactors(1,1,1))
combination_factors.insert("vehículos_pesados",loadCombinations.CombinationFactors(0.75,0.75,0))
combination_factors.insert("sobrecarga_uniforme",loadCombinations.CombinationFactors(0.4,0.4,0))
combination_factors.insert("carga_en_aceras",loadCombinations.CombinationFactors(0.4,0.4,0))
#
combination_factors.insert("f_horizontal",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("peatones",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("aglomeraciones",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("SCuso_pasarelas",loadCombinations.CombinationFactors(0.4,0.4,0))
#Viento
combination_factors.insert("viento_sit_persistente",loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert("viento_sit_construccion",loadCombinations.CombinationFactors(0.8,0.0,0.0))
combination_factors.insert("viento_pasarelas",loadCombinations.CombinationFactors(0.3,0.2,0.0))
# Acción térmica
combination_factors.insert("termica",loadCombinations.CombinationFactors(0.6,0.6,0.5))
# Nieve
combination_factors.insert("nieve_construccion",loadCombinations.CombinationFactors(0.8,0.0,0.0))
#Acción del agua
combination_factors.insert("emp_hidrost",loadCombinations.CombinationFactors(1.0,1.0,1.0))
combination_factors.insert("emp_hidrodin",loadCombinations.CombinationFactors(1.0,1.0,1.0))
#Sobrecargas de construcción
combination_factors.insert("sc_construc",loadCombinations.CombinationFactors(1.0,0.0,1.0))

combination_factors.insert("por_defecto",loadCombinations.CombinationFactors(0.7,0.7,0.6))



controlCombGenerator= loadCombinations.LoadCombGenerator()
actionsAndFactors= controlCombGenerator.actionWeighting.create("IAP11",factors)

