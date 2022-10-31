# -*- coding: utf-8 -*-
''' Load combinations according to SIA-260 (Switzerland).'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import loadCombinations
from actions.load_combination_utils import utils

factors= loadCombinations.Factors()
partial_safety_factors= factors.getPartialSafetyFactors()

#SIA 260 Tableau 1 (chiffre 4.4.3)
#Partial safety factors for permanent actions
partial_safety_factors["permanentes"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.8,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Poussée des terres.
partial_safety_factors["terres"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0,1.35/0.7,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
#Partial safety factors for variable actions
partial_safety_factors["variables"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
partial_safety_factors["trafic_ferroviaire"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.45,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
partial_safety_factors["trafic_ferroviaire_model_3"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.20,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Partial safety factors for seismic actions
partial_safety_factors["sismicas"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))
#Partial safety factors for accidental actions.
partial_safety_factors["accidentales"]= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,1,1),loadCombinations.SLSPartialSafetyFactors(0,0))


combination_factors= factors.getCombinationFactors()
#Tableau 2 SIA 260 ANNEXE A Bâtiments.
combination_factors.insert("permanent",loadCombinations.CombinationFactors(1,1,1))
combination_factors.insert("categorie_A",loadCombinations.CombinationFactors(0.7,0.5,0.3)) #Locaux habitables
combination_factors.insert("categorie_B",loadCombinations.CombinationFactors(0.7,0.5,0.3)) #Locaux administratifs.
combination_factors.insert("categorie_C",loadCombinations.CombinationFactors(0.7,0.7,0.6)) #Locaux réunion.
combination_factors.insert("categorie_D",loadCombinations.CombinationFactors(0.7,0.7,0.6)) #Locaux de vente.
combination_factors.insert("categorie_E",loadCombinations.CombinationFactors(1.0,0.9,0.8)) #Entrepôts.
combination_factors.insert("categorie_F",loadCombinations.CombinationFactors(0.7,0.7,0.6)) #Véhicules en dessous de 3.5t.
combination_factors.insert("categorie_G",loadCombinations.CombinationFactors(0.7,0.5,0.3)) #Véhicules de 3.5t à 16 t.
combination_factors.insert("categorie_H",loadCombinations.CombinationFactors(0.0,0.0,0.0)) #Toits.
combination_factors.insert("vent",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("temperature",loadCombinations.CombinationFactors(0.6,0.5,0.0))
combination_factors.insert("poussee_terres",loadCombinations.CombinationFactors(0.7,0.7,0.7))
combination_factors.insert("pression_hydraulique",loadCombinations.CombinationFactors(0.7,0.7,0.7))

combination_factors.insert("voie_normale_load_model_1",loadCombinations.CombinationFactors(1.0,1.0,0.0))
combination_factors.insert("voie_normale_load_model_2",loadCombinations.CombinationFactors(1.0,1.0,0.0))
combination_factors.insert("voie_normale_load_model_3",loadCombinations.CombinationFactors(0.0,1.0,0.0))
combination_factors.insert("force_aerodynamiques_train",loadCombinations.CombinationFactors(1.0,0.5,0.0))

combination_factors.insert("voie_etroite_load_model_4",loadCombinations.CombinationFactors(1.0,1.0,0.3))
combination_factors.insert("voie_etroite_load_model_5",loadCombinations.CombinationFactors(1.0,1.0,0.3))
combination_factors.insert("voie_etroite_load_model_6",loadCombinations.CombinationFactors(0.0,1.0,0.0))
combination_factors.insert("voie_etroite_load_model_7",loadCombinations.CombinationFactors(0.0,1.0,0.0))
combination_factors.insert("voie_etroite_load_model_8",loadCombinations.CombinationFactors(0.0,1.0,0.0))
combination_factors.insert("voie_etroite_choc",loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert("voie_etroite_vent",loadCombinations.CombinationFactors(0.8,0.5,0.0))
combination_factors.insert("voie_etroite_force_aerodynamiques_train",loadCombinations.CombinationFactors(1.0,0.5,0.0))
combination_factors.insert("voie_etroite_temperature",loadCombinations.CombinationFactors(0.6,0.6,0.5))
combination_factors.insert("voie_etroite_poussee_terres",loadCombinations.CombinationFactors(0.7,0.7,0.7))
combination_factors.insert("voie_etroite_pression_hydraulique",loadCombinations.CombinationFactors(0.7,0.7,0.7))

combination_factors.insert("ponts_routes_load_model_1",loadCombinations.CombinationFactors(0.75,0.75,0.0))
combination_factors.insert("ponts_routes_load_model_3",loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert("ponts_routes_choc",loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert("ponts_routes_neige",loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert("ponts_routes_vent",loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert("ponts_routes_temperature",loadCombinations.CombinationFactors(0.6,0.6,0.5))
combination_factors.insert("ponts_routes_poussee_terres",loadCombinations.CombinationFactors(0.7,0.7,0.7))
combination_factors.insert("ponts_routes_pression_hydraulique",loadCombinations.CombinationFactors(0.7,0.7,0.7))

combination_factors.insert("accidentales",loadCombinations.CombinationFactors(1,1,1))

class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to SIA 260.'''

    def __init__(self):
        ''' Constructor.'''
        super().__init__(combGeneratorName= 'SIA260', factors= factors)

    def newPermanentAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "permanentes",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= "permanentes", dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newRailwayTrafficAction(self, actionName: str, actionDescription: str, combinationFactorsName:str, dependsOn= None, incompatibleActions= None):
        ''' Creates a railway traffic action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= "trafic_ferroviaire", dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newWindAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a wind action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'vent', partialSafetyFactorsName= "variables", dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newThermalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates a wind action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        '''
        return self.newAction(family= "variables",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'temperature', partialSafetyFactorsName= "variables", dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newAccidentalAction(self, actionName: str, actionDescription: str, combinationFactorsName:str, dependsOn= None, incompatibleActions= None):
        ''' Creates an accidental action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        return self.newAction(family= "accidentales",actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= "accidentales", dependsOn= dependsOn, incompatibleActions= incompatibleActions)

combGenerator= CombGenerator()
        
