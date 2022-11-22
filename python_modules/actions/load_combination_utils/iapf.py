# -*- coding: utf-8 -*-
#import sys
#sys.path.append('usr/local/src/fuentes/c/biblioteca/xc_utils/lib/')

import loadCombinations
from actions.load_combination_utils import utils

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#Coeficientes de ponderación para acciones permanentes.
partial_safety_factors["permanentes"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P1 (armaduras postesas).
partial_safety_factors["permanentes_nc_Pret1Post"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1,1,1),loadCombinations.SLSPartialSafetyFactors(0.9,1.1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P1 (armaduras pretesas).
partial_safety_factors["permanentes_nc_Pret1Pret"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1,1,1),loadCombinations.SLSPartialSafetyFactors(0.95,1.05))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas al pretensado P2.
partial_safety_factors["permanentes_nc_Pret2"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a efectos reológicos (retracción y fluencia).
partial_safety_factors["permanentes_nc_Reol"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a acciones provocadas por el balasto.
partial_safety_factors["permanentes_nc_CMBalasto"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a acciones provocadas por empujes o movimientos del terreno.
partial_safety_factors["permanentes_nc_Terr"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante debidas a otras presolicitaciones distintas del pretensado.
partial_safety_factors["permanentes_nc_OtrasPresol"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.95,1.05,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables.
partial_safety_factors["variables"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones sísmicas.
partial_safety_factors["sismicas"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Coeficientes de ponderación para acciones accidentales.
partial_safety_factors["accidentales"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))

combination_factors= factors.getCombinationFactors()

combination_factors.insert("permanent",loadCombinations.CombinationFactors(1,1,1))
combination_factors.insert("snow",loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert("wind",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("thermal",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("sismo",loadCombinations.CombinationFactors(0,0,0))
combination_factors.insert("arranque_frenado",loadCombinations.CombinationFactors(0.6,0.2,0))
combination_factors.insert("por_defecto",loadCombinations.CombinationFactors(0.7,0.7,0.6))


class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to IAP-11.'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'IAPF', factors= factors)
        
    def newPermanentAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
    def newRailwayTrafficAction(self, actionName: str, actionDescription: str, combinationFactorsName:str, dependsOn= None, incompatibleActions= None):
        ''' Creates a railway traffic action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
    def newWindAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a wind action on footbridge and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
    def newThermalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a thermal action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None

    def newSnowAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
    def newSeismicAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
combGenerator= CombGenerator()
