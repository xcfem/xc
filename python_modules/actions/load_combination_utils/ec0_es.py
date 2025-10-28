# -*- coding: utf-8 -*-
''' Load combinations according to Spanish Annex for Eurocode 0.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2015, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from actions.load_combination_utils import utils
from actions.load_combination_utils import ec0_bridges_partial_safety_factors_es
from actions.load_combination_utils import ec0_bridges_combination_factors_es
from actions.load_combination_utils import ec0_buildings_partial_safety_factors_es
from actions.load_combination_utils import ec0_buildings_combination_factors_es
from misc_utils import log_messages as lmsg


#Coef.FULS('favorable','desfavorable','favorable accidental','desfavorable accidental')

# Excerpt from clause 6.4.1. The following ultimate limit states shall be verified as relevant:

#  - EQU: Loss of static equilibrium of the structure or any part of it considered as a rigid body.
#  - STR: Internal failure or excessive deformation of the structure or structural members, including footings, piles, basement walls, etc., where the strength of construction materials of the structure governs.
#  - GEO: Failure or excessive deformation of the ground where the strengths of soil or rock are significant in providing resistance.
#  - FAT: Fatigue failure of the structure or structural members.
#  - UPL: loss of equilibrium of the structure or the ground due to uplift by water pressure (buoyancy) or other vertical actions.
#  - HYD: hydraulic heave, internal erosion and piping in the ground caused by hydraulic gradients.

# Design values of actions (on bridges).
#  * Set A: Static equilibrium (EQU) on bridges should be verified using the design values of actions in Table A2.4(A).
#  * Set B: Design of bridge structural members (STR) not involving geotechnical actions should be verified using the design values of actions in Table A2.4(B).
#  * Set C: Design of structural members (footings, piles, piers, side walls, wing walls, flank walls and front walls of abutments, ballast retention walls, etc.) (STR) involving geotechnical actions and the resistance of the ground (GEO) should be verified using one only of the following three approaches supplemented, for geotechnical actions and resistances, by EN 1997:
#      - Approach 1: Applying in separate calculations design values from Table A2.4(C) and Table A2.4(B) to the geotechnical actions as well as the actions on/from the structure.
#      - Approach 2: Applying design values of actions from Table A2.4(B) to the geotechnical actions as well as the actions on/from the structure;
#      - Approach 3: Applying design values of actions from Table A2.4(C) to the geotechnical actions and, simultaneously, applying design values of actions from Table A2.4(B) to the actions on/from the structure.
 


# Importance factors for bridges (See Spanish National Annex AN/UNE-EN 1998-2 clause 2.1(6))
lessImportantBridgesImportanceFactor= None # Must be fixed by the competent authority.
normalBridgesImportanceFactor= 1.0
specialBridgesImportanceFactor= 1.3

class BridgeCombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to Eurocode 0 (Spanish annex).

    :ivar structureType: structure type (road_bridge or footbridge or railway_bridge).
    '''

    def __init__(self, structureType= 'road_bridge'):
        ''' Constructor.

        :param structureType: structure type (road_bridge or footbridge or railway_bridge or building).
        '''
        factors= loadCombinations.Factors()
        
        # Partial safety factors for bridges.
        partial_safety_factors= factors.getPartialSafetyFactors()
        ec0_bridges_partial_safety_factors_es._set_bridges_partial_safety_factors(partial_safety_factors)

        # Combination factors for road bridges (table AN.5 (table A2.1) )
        combination_factors= factors.getCombinationFactors()
        ec0_bridges_combination_factors_es._set_bridges_combination_factors(combination_factors)

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
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= className+'.'+methodName+'; seismic partical safety factors for ulsImportanceFactor=  '+str(ulsImportanceFactor) + ' and slsImportanceFactor= '+str(slsImportanceFactor) + ' already defined. Command ignored.'
            lmsg.warning(warningMsg)
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
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        warningMsg= className+'.'+methodName+'; has been DEPRECATED, specify the nature of the action using newSelfWeightAction or newDeadLoadAction or ...'
        lmsg.warning(warningMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= 'permanent_set_b', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        
    def newSelfWeightAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a self weight action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'self_weight_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'self_weight_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        
    def newDeadLoadAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a dead load action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'dead_load_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'dead_load_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        
    def newEarthPressureAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a earth pressure action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'earth_pressure_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'earth_pressure_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
               
    def newPoreWaterPressureAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a pore water pressure action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'pore_water_pressure_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'pore_water_pressure_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for pore water pressure.'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        
    def newRheologicalAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a rheological action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'rheological_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for rheological actions.'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
        
    def newFrictionInSlidingSupports(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a friction in sliding support action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'friction_in_sliding_supports_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for rheological actions.'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
     
    def newSettlementAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, nonLinearAnalysis= True, context= None, notDeterminant= False):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        if(nonLinearAnalysis):
            partialSafetyFactorsName= 'settlement_non_linear_analysis'
        else:
            partialSafetyFactorsName= 'settlement_linear_analysis'
        if(safetyFactorSet=='B'):
            partialSafetyFactorsName+= '_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for rheological actions.'
            lmsg.error(errorMsg)        
        return self.newAction(family= 'variable',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
    
    def newFreeWaterPressureAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a free water pressure action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway bridge, footbridge,...)
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'free_water_pressure_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'free_water_pressure_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for free water pressure.'
            lmsg.error(errorMsg)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newHeavyVehicleAction(self, actionName: str, actionDescription: str, group:str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= 'road_bridge', notDeterminant= False):
        ''' Creates a heavy vehicle action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param group: load group (gr1a, gr1b, gr4 or gr5).
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'road_traffic_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'road_traffic_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for trafficactions.'
            lmsg.error(errorMsg)        
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

            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= cFactors, partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
    
    def newUniformLoadAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= 'road_bridge', notDeterminant= False):
        ''' Creates a uniformly distributed load (UDL system) and appends it 
            to the combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(context=='road_bridge'): # uniformly distributed load (UDL system)
            partialSafetyFactorsName= 'road_traffic'
        elif(context=='load_on_backfill'):
            partialSafetyFactorsName= context
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName+= '_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName+= '_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for traffic actions.'
            lmsg.error(errorMsg)                
        retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_traffic_loads_gr1a_udl', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        return retval
    
    def newRailwayTrafficAction(self, actionName:str, actionDescription:str, combinationFactorsName:str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= 'railway_bridge', notDeterminant= False):
        ''' Creates a railway traffic action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param combinationFactorsName: name of the combination factors container.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'railway_traffic_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'railway_traffic_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                
        if(context=='railway_bridge'):
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
        
    def newLoadOnBackfillAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None, notDeterminant= False):
        ''' Creates a wind action on bridge and appends it to the combinations
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'load_on_backfill_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'load_on_backfill_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_construction', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        elif(context=='railway_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'railway_bridge_construction', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
            exit(1)
        return retval
    
    def newConstructionAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None, notDeterminant= False):
        ''' Creates a wind action on bridge and appends it to the combinations
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'construction_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'construction_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_construction', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        elif(context=='railway_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'railway_bridge_construction', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
            exit(1)
        return retval
    
    def newWindAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None, notDeterminant= False):
        ''' Creates a wind action on bridge and appends it to the combinations
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'variable_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'variable_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_wind_persistent_situation', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        elif(context=='railway_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'railway_bridge_wind_persistent_situation', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval
    
    def newThermalAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= 'road_bridge', notDeterminant= False):
        ''' Creates a thermal action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'thermal_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'thermal_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_thermal', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        elif(context=='railway_bridge'):
            retval= self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'railway_bridge_thermal', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval        

    def newHydrostaticPressureAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= 'road_bridge', notDeterminant= False):
        ''' Creates a hydrostatic pressure action and appends it to the 
            combinations generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        :param notDeterminant: set to True if action cannot be determinant, otherwise it must be False.
        '''
        retval= None
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName= 'hydrostatic_pressure_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName= 'hydrostatic_pressure_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        if(context=='road_bridge'):
            retval= self.newAction(family= 'variables', actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'road_bridge_hydrostatic_pressure', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions, notDeterminant= notDeterminant)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for context: '+str(context) + ' return None.')
        return retval    
    
    def newSnowAction(self, actionName: str, actionDescription: str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a snow action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        raise NotImplementedError()
        return None
    
    def newAccidentalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates an accidental action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''

        return self.newAction(family= "accidental", actionName= actionName, actionDescription= actionDescription, combinationFactorsName= '', partialSafetyFactorsName= 'accidentales', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newSeismicAction(self, actionName: str, actionDescription: str, ulsImportanceFactor, slsImportanceFactor= 0.0, dependsOn= None, incompatibleActions= None, context= None):
        ''' Creates a seismic action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param ulsImportanceFactor: importance factor (partial safety factor)
                                    to use with seismic action in ultimate
                                    limit states. The importance factor is used
                                    to introduce in the analysis the differences
                                    in the return period according to the importance
                                    class of the structure. See EC-8 part 1 clauses 2.1 
                                    and 4.2.5 and EC-8 part 2 clause 2.1.
        :param slsImportanceFactor: importance factor (partial safety factor) 
                                    to use with seismic action in
                                    serviceability limit states. The importance factor is used
                                    to introduce in the analysis the differences
                                    in the return period according to the importance
                                    of the structure. See EC-8 part 1 clauses 2.1 
                                    and 4.2.5 and EC-8 part 2 clause 2.1.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        :param context: context for the action (building, railway_bridge, footbridge,...)
        '''
        code= "seismic_"+str(ulsImportanceFactor)+"_"+str(slsImportanceFactor)
        partial_safety_factors= self.getPartialSafetyFactors().getKeys()
        if not code in partial_safety_factors: # Part. sfty fctrs not yet defnd
            self.defSeismicPartialSafetyFactors(ulsImportanceFactor, slsImportanceFactor)
        return self.newAction(family= "seismic", actionName= actionName, actionDescription= actionDescription, combinationFactorsName= '', partialSafetyFactorsName= code, dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newActionGroup(self, family: str, actionTuples, partialSafetyFactorsName:str, safetyFactorSet:str, dependsOn= None, incompatibleActions= None):
        ''' Creates an action and appends it to the combinations generator.

        :param family: family to which the action belongs ("permanent", "variable", "seismic",...)
        :param actionTuples: tuples of (name, description, combinationFactorsName) defining the actions of the group.
        :param partialSafetyFactorsName: name of the partial safety factor container.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' or 'C' corresponding to tables Table A2.4(A), Table A2.4(B) or A2.4(C)
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        if(safetyFactorSet=='A'):
            partialSafetyFactorsName+= '_set_a'
        elif(safetyFactorSet=='B'):
            partialSafetyFactorsName+= '_set_b'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented for railway traffic actions.'
            lmsg.error(errorMsg)                        
        return super().newActionGroup(family= family, actionTuples= actionTuples, partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
bridgeCombGenerator= BridgeCombGenerator()


class BuildingCombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to Eurocode 0 (Spanish annex).

    :ivar altitude: altitude of the building roof (meters above sea level).
    :ivar cen_state: CEN member state.
    '''

    def __init__(self, altitude:float, cen_state= 'Spain'):
        ''' Constructor.

        :param altitude: altitude of the roof (meters above sea level).
        :param cen_state: CEN member state.
        '''
        self.altitude= altitude
        self.cen_state= cen_state
        factors= loadCombinations.Factors()
        
        # Partial safety factors for buildings.
        partial_safety_factors= factors.getPartialSafetyFactors()
        ec0_buildings_partial_safety_factors_es._set_buildings_partial_safety_factors(partial_safety_factors)

        # Combination factors for road buildings (table AN.5 (table A2.1) )
        combination_factors= factors.getCombinationFactors()
        ec0_buildings_combination_factors_es._set_buildings_combination_factors(combination_factors, altitude= self.altitude, cen_state= self.cen_state)
        
        super().__init__(combGeneratorName= 'EC0_ES', factors= factors)

    def getPartialSafetyFactorsName(self, permanent:bool, safetyFactorSet:str, designSituation:str, approach:int= 2):
        ''' Return the name of the partial safety factors to use in the given
            condition.

 
        :param permanent: if true return the partial safety factors for 
                          permanent loads otherwise return those corresponding
                          to variable ones.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        '''
        retval= None
        if(approach!=2):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; design approach: '+str(approach)+' not implemented yet.'
            lmsg.error(errorMsg)
            exit(1)
        if(designSituation=='EQU'):
            if(safetyFactorSet=='A'):
                retval= 'permanent_equ_set_a'
            elif(safetyFactorSet=='B'):
                retval= 'permanent_equ_set_b'
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented'
                lmsg.error(errorMsg)
        elif(designSituation in ['STR', 'GEO', 'STR/GEO']):
            if(safetyFactorSet=='A'):
                retval= 'permanent_str/geo_set_a'
            elif(safetyFactorSet=='B'):
                retval= 'permanent_str/geo_set_b'
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= className+'.'+methodName+'; safety factor set: '+str(safetyFactorSet)+' not implemented'
                lmsg.error(errorMsg)
                exit(1)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; design situation: '+str(designSituation)+' not implemented.'
            lmsg.error(errorMsg)
            exit(1)
        if(not permanent):
            retval= retval.replace('permanent_', 'variable_')
        return retval
        
    def newPermanentAction(self, actionName: str, actionDescription:str, safetyFactorSet:str, designSituation:str, approach:int= 2, dependsOn= None, incompatibleActions= None):
        ''' Creates a permanent action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        partialSafetyFactorsName= self.getPartialSafetyFactorsName(permanent= True, safetyFactorSet= safetyFactorSet, designSituation= designSituation, approach= approach)
        return self.newAction(family= 'permanent',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'permanent', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newImposedLoadAction(self, actionName: str, actionDescription:str, imposedLoadType, safetyFactorSet:str, designSituation:str, approach:int= 2, dependsOn= None, incompatibleActions= None):
        ''' Creates an imposed load action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param imposedLoadType: one of the following types: 'residential',
                                'office', 'congregation', 'shopping', 'storage',
                                'ligth_vehicles', 'heavy_vehicles'
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        partialSafetyFactorsName= self.getPartialSafetyFactorsName(permanent= False, safetyFactorSet= safetyFactorSet, designSituation= designSituation, approach= approach)
        keys= self.getCombinationFactors().getKeys()
        if(imposedLoadType in keys):
            combinationFactorsName= key
        elif(imposedLoadType=='residential'):
            combinationFactorsName= 'cat_a_domestic_residential_areas'
        elif(imposedLoadType=='office'):
            combinationFactorsName= 'cat_c_congregation_areas'
        elif(imposedLoadType=='congregation'):
            combinationFactorsName= 'cat_c_congregation_areas'
        elif(imposedLoadType=='shopping'):
            combinationFactorsName= 'cat_D_shopping_areas'
        elif(imposedLoadType=='storage'):
            combinationFactorsName= 'cat_E_storage_areas'
        elif(imposedLoadType=='light_vehicles'):
            combinationFactorsName= 'cat_F_traffic_area_vehicle_weight_leq_30kN'
        elif(imposedLoadType=='heavy_vehicles'):
            combinationFactorsName= 'cat_G_traffic_area_30kN_lt_vehicle weight_leq_160kN'
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= className+'.'+methodName+'; imposed load type: '+str(imposedLoadType)+' not implemented.'
            lmsg.error(errorMsg)
            exit(1)
        return self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= combinationFactorsName, partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newWindLoadAction(self, actionName: str, actionDescription:str, safetyFactorSet:str, designSituation:str, approach:int= 2, dependsOn= None, incompatibleActions= None):
        ''' Creates a wind load action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        partialSafetyFactorsName= self.getPartialSafetyFactorsName(permanent= False, safetyFactorSet= safetyFactorSet, designSituation= designSituation, approach= approach)
        return self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'wind_loads_on_buildings', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newSnowLoadAction(self, actionName: str, actionDescription:str, safetyFactorSet:str, designSituation:str, approach:int= 2, dependsOn= None, incompatibleActions= None):
        ''' Creates a snow load action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        partialSafetyFactorsName= self.getPartialSafetyFactorsName(permanent= False, safetyFactorSet= safetyFactorSet, designSituation= designSituation, approach= approach)
        return self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'cat_H_snow_loads_on_building_roofs', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
    def newThermalLoadAction(self, actionName: str, actionDescription:str, safetyFactorSet:str, designSituation:str, approach:int= 2, dependsOn= None, incompatibleActions= None):
        ''' Creates a thermal load action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param safetyFactorSet: identifier of the safety factor set 'A', 'B' 
                                or 'C' corresponding to tables Table A1.2(A), 
                                Table A1.2(B) or A1.2(C)
        :param designSituation: design situation according to clause AI.3.1: 
                                'EQU', 'STR', 'GEO' or 'STR/GEO'.
        :param approach: aproach for the design of structural members involving
                         geotechnical actions and the resistance of the ground
                         (footings, piles, basement walls, etc.). See paragraph
                         (5) of the clause A1.3.1 of Eurocode 0.
        :param dependsOn: name of another load that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''
        partialSafetyFactorsName= self.getPartialSafetyFactorsName(permanent= False, safetyFactorSet= safetyFactorSet, designSituation= designSituation, approach= approach)
        return self.newAction(family= 'variables',actionName= actionName, actionDescription= actionDescription, combinationFactorsName= 'temperature_in_buildings', partialSafetyFactorsName= partialSafetyFactorsName, dependsOn= dependsOn, incompatibleActions= incompatibleActions)

    def newAccidentalAction(self, actionName: str, actionDescription: str, dependsOn= None, incompatibleActions= None):
        ''' Creates an accidental action and appends it to the combinations 
            generator.

        :param actionName: name of the action.
        :param actionDescription: description of the action.
        :param dependsOn: name of another action that must be present with this one (for example brake loads depend on traffic loads).
        :param incompatibleActions: list of regular expressions that match the names of the actions that are incompatible with this one.
        '''

        return self.newAction(family= "accidental", actionName= actionName, actionDescription= actionDescription, combinationFactorsName= '', partialSafetyFactorsName= 'accidentales', dependsOn= dependsOn, incompatibleActions= incompatibleActions)
    
