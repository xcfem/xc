# -*- coding: utf-8 -*-
''' Load combinations according to Spanish EHE-08.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import utils

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


class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to IAP-11.'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'EHEIntenso', factors= factors)
        
    def newPermanentAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "permanentes",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanents', partialSafetyFactorsName= "permanentes_ctr_intenso", dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newVariableAction(self, actionName: str, actionDescription: str, combinationFactorsName:str, dependsOn= None, incompatibleActions= None, notDeterminant= False):
        ''' Creates an variable action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= "variables_ctr_intenso", dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)    
    
    def newAccidentalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates an accidental action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "accidentales",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= "accidental", partialSafetyFactorsName= "accidentales_ctr_intenso", dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
combGenerator= CombGenerator()
