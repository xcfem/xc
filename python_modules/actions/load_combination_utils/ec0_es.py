# -*- coding: utf-8 -*-
''' Load combinations according to Spanish IAP-11.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2015, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from actions.load_combination_utils import utils
from misc_utils import log_messages as lmsg

factors= loadCombinations.Factors()

#Coef.FULS('favorable','desfavorable','favorable accidental','desfavorable accidental')


#Partial safety factors EC0
# -Persistent and transient situations: table AN.9 (A2.4(B) (STR/GEO) (Set B) )
# -Accidental situations: table AN.10 (table A2.5).
partial_safety_factors= factors.getPartialSafetyFactors()

#Partial safety factors for permanent actions
partial_safety_factors['permanent']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1),loadCombinations.SLSPartialSafetyFactors(1,1))
partial_safety_factors['settlement_linear_analysis']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.20,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
partial_safety_factors['settlement_non_linear_analysis']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
#Partial safety factors for variable actions.
# Traffic loads.
partial_safety_factors['road_traffic']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
partial_safety_factors['pedestrian']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
partial_safety_factors['railway_traffic']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.45,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
# Thermal actions.
partial_safety_factors['thermal']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))
# Hydrostatic pressure
partial_safety_factors['hydrostatic_pressure']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1),loadCombinations.SLSPartialSafetyFactors(0,1))

# Partial safety factors for accidental actions.
partial_safety_factors['accidentales']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,0,1),loadCombinations.SLSPartialSafetyFactors(0,0))

# Combination factors for road bridges (table AN.5 (table A2.1) )
combination_factors= factors.getCombinationFactors()
# 
combination_factors.insert('permanent',loadCombinations.CombinationFactors(1,1,1))
combination_factors.insert('road_traffic_loads_gr1a_trucks',loadCombinations.CombinationFactors(0.75,0.75,0.0))
combination_factors.insert('road_traffic_loads_gr1a_udl',loadCombinations.CombinationFactors(0.40,0.40,0.0))
combination_factors.insert('road_traffic_loads_gr1a_footway',loadCombinations.CombinationFactors(0.40,0.40,0.0))
combination_factors.insert('road_traffic_loads_gr1b_single_axe',loadCombinations.CombinationFactors(0.0,0.75,0.0))
combination_factors.insert('road_traffic_loads_gr2_horizontal_forces',loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert('road_traffic_loads_gr3_pedestrian_loads',loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert('road_traffic_loads_gr4_crowd_loading',loadCombinations.CombinationFactors(0.0,0.0,0.0))
combination_factors.insert('road_traffic_loads_gr5_vertical_forces',loadCombinations.CombinationFactors(0.0,0.0,0.0)) # Special vehicles. Vertical forces.
combination_factors.insert('road_traffic_loads_gr6_horizontal_forces',loadCombinations.CombinationFactors(0.0,0.0,0.0)) # Special vehicles. Horizontal forces.
combination_factors.insert('road_bridge_wind_persistent_situation',loadCombinations.CombinationFactors(0.6,0.2,0.0))
combination_factors.insert('road_bridge_wind_during_execution',loadCombinations.CombinationFactors(0.8,0.0,0.0))
combination_factors.insert('road_bridge_wind_aster',loadCombinations.CombinationFactors(1.0,0.0,0.0))
combination_factors.insert('road_bridge_thermal',loadCombinations.CombinationFactors(0.6,0.6,0.5))
combination_factors.insert('road_bridge_snow',loadCombinations.CombinationFactors(0.8,0.0,0.0))
combination_factors.insert('road_bridge_hydrostatic_pressure',loadCombinations.CombinationFactors(1.0,1.0,1.0))
combination_factors.insert('road_bridge_construction_loads',loadCombinations.CombinationFactors(1.0,0.0,1.0))
# Combination factors for railway bridges
combination_factors.insert('LM71_alone_uls',loadCombinations.CombinationFactors(0.8,0.8,0.0))

class CombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to Eurocode 0 (Spanish annex).

    :ivar structureType: structure type (road_bridge or footbridge or railway_bridge or building).
    '''

    def __init__(self, structureType= 'road_bridge'):
        ''' Constructor.

        :param structureType: structure type (road_bridge or footbridge or railway_bridge or building).
        '''
        super().__init__(combGeneratorName= 'EC0_ES', factors= factors)
        self.structureType= structureType
        
    def defSeismicPartialSafetyFactors(self, ulsImportanceFactor, slsImportanceFactor= 0.0):
        ''' Create the partial safety factors for the seismic actions.

        :param ulsImportanceFactor: importance factor (partial safety factor) to use
                                    with seismic actions in ultimate limit states.
        :param slsImportanceFactor: importance factor (partial safety factor) to use
                                    with seismic actions in serviceability limit
                                    states.
        '''
        partial_safety_factors= self.getPartialSafetyFactors()
        code= "seismic_"+str(ulsImportanceFactor)+"_"+str(slsImportanceFactor)
        retval= None
        if code in partial_safety_factors:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; seismic partical safety factors for ulsImportanceFactor=  '+str(ulsImportanceFactor) + ' and slsImportanceFactor= '+str(slsImportanceFactor) + ' already defined. Command ignored.')
        else:
            psf= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,0,ulsImportanceFactor), loadCombinations.SLSPartialSafetyFactors(0,slsImportanceFactor))
            partial_safety_factors.insert(code,psf)
        return code
        
    def newPermanentAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= 'permanent', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newSettlementAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, nonLinearAnalysis= True, context= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        if(nonLinearAnalysis):
            partialSafetyFactorsName= 'settlement_non_linear_analysis'
        else:
            partialSafetyFactorsName= 'settlement_linear_analysis'
        return self.newAction(family= 'variable',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newHeavyVehicleAction(self, actionName: str, actionDescription: str, group:str, dependsOn= None, incompatibleActions= None, context= 'road_bridge'):
        ''' Creates a heavy vehicle action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param group: load group (gr1a, gr1b, gr4 or gr5).
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        retval= None
        if(context=='road_bridge'):
            cFactors= 'road_traffic_loads_gr1a_trucks'
            if(group=='gr1a'):
                cFactors= 'road_traffic_loads_gr1a_trucks'
            elif(group=='gr1b'):
                cFactors= 'road_traffic_loads_gr1b_single_axe'
            elif(group=='gr4'):
                cFactors= 'road_traffic_loads_gr4_crowd_loading'
            elif(group=='gr5'): # Special vehicles.
                cFactors= 'road_traffic_loads_gr5_vertical_forces'
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; group: '+str(group) + ' doesn\'t correspond to vertical loads from heavy vehicles (punctual loads).')

            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= cFactors, partialSafetyFactorsName= 'road_traffic', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
    
    def newUniformLoadAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= 'road_bridge'):
        ''' Creates a uniformly distributed load (UDL system) and appends it 
            to the combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        retval= None
        if(context=='road_bridge'): # uniformly distributed load (UDL system)
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_traffic_loads_gr1a_udl', partialSafetyFactorsName= 'road_traffic', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
    
    def newRailwayTrafficAction(self, actionName:str, actionDescription:str, combinationFactorsName:str, dependsOn= None, incompatibleActions= None, context= 'railway_bridge'):
        ''' Creates a railway traffic action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        retval= None
        if(context=='railway_bridge'):
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= 'railway_traffic', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
        
    
    def newWindAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a wind action on footbridge and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        raise NotImplementedError()
        return None
    
    def newThermalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= 'road_bridge'):
        ''' Creates a thermal action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        retval= None
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_thermal', partialSafetyFactorsName= 'thermal', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval        

    def newHydrostaticPressureAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= 'road_bridge'):
        ''' Creates a thermal action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        retval= None
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_hydrostatic_pressure', partialSafetyFactorsName= 'hydrostatic_pressure', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval    
    
    def newSnowAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        raise NotImplementedError()
        return None
    
    def newSeismicAction(self, actionName: str, actionDescription: str, ulsImportanceFactor, slsImportanceFactor= 0.0, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param ulsImportanceFactor: importance factor (partial safety factor)
                                    to use with seismic action in ultimate
                                    limit states.
        :param slsImportanceFactor: importance factor (partial safety factor) 
                                    to use with seismic action in
                                    serviceability limit states.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        code= "seismic_"+str(ulsImportanceFactor)+"_"+str(slsImportanceFactor)
        if not code in partial_safety_factors: # Part. sfty fctrs not yet defnd
            self.defSeismicPartialSafetyFactors(ulsImportanceFactor, slsImportanceFactor)
        return self.newAction(family= "seismic", actionName= actionName, actionDescription= actionDescription, combinationFactorsName= '', partialSafetyFactorsName= code, dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    
combGenerator= CombGenerator()
