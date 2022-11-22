# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import utils

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#Coeficientes de ponderación para acciones permanentes.
partial_safety_factors["permanentes_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,1.6,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante.
partial_safety_factors["permanentes_nc_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,0.75*1.6,0,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables.
partial_safety_factors["variables_42IC"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.6,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))

class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to 42IC.'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= '42IC', factors= factors)
        
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
    
    def newHeavyVehicleAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a heavy vehicle action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        raise NotImplementedError()
        return None
    
    def newFootbridgeAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a footbridge action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
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

