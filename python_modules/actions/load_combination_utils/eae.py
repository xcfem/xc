# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#Coeficientes de ponderación para acciones permanentes.
partial_safety_factors["permanentes"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones permanentes de valor no constante.
partial_safety_factors["permanentes_nc"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.5,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Coeficientes de ponderación para acciones variables.
partial_safety_factors["variables"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Coeficientes de ponderación para acciones sísmicas.
partial_safety_factors["sismicas"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Coeficientes de ponderación para acciones accidentales.
partial_safety_factors["accidentales"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))

combination_factors= factors.getCombinationFactors()

combination_factors.insert("zonas_residenciales",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("zonas_oficinas",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("zonas_reunion",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("zonas_comerciales",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("zonas_almacen",loadCombinations.CombinationFactors(1.0,0.9,0.8))
combination_factors.insert("zonas_trafico_veh_ligeros",loadCombinations.CombinationFactors(0.7,0.7,0.6))
combination_factors.insert("zonas_trafico_veh_pesados",loadCombinations.CombinationFactors(0.7,0.5,0.3))
combination_factors.insert("cubiertas_inaccesibles",loadCombinations.CombinationFactors(0.0,0.0,0.0))


class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to EAE (Spain).'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'EAE', factors= factors)
        
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
