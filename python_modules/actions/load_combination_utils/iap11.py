# -*- coding: utf-8 -*-
''' Load combinations according to Spanish IAP-11.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import utils

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

# Factores de simultaneidad (tabla 6.1-a)
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

class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to IAP-11.'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'IAP11', factors= factors)
        
    def newPermanentAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "permanentes",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanentes', partialSafetyFactorsName= "permanentes", dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newHeavyVehicleAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates a heavy vehicle action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'vehículos_pesados', partialSafetyFactorsName= "variables_SCuso", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)

    def newFootbridgeAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates a footbridge action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'SCuso_pasarelas', partialSafetyFactorsName= "variables_SCuso", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
    
    def newFootbridgeWindAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates a wind action on footbridge and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'viento_pasarelas', partialSafetyFactorsName= "variables_climatica", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
    
    def newThermalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates a thermal action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'termica', partialSafetyFactorsName= "variables_climatica", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)

    def newSnowAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'nieve_construccion', partialSafetyFactorsName= "variables_climatica", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
    
    def newSeismicAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "sismicas",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= '', partialSafetyFactorsName= "sismicas", dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
combGenerator= CombGenerator()

    
