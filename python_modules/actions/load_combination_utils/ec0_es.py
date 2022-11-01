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

class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to Eurocode 0 (Spanish annex).'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'EC0_ES', factors= factors)
        
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
