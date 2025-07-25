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
from misc_utils import log_messages as lmsg

bridge_factors= loadCombinations.Factors()

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
 

# Partial safety factors for bridges.
bridges_partial_safety_factors= bridge_factors.getPartialSafetyFactors()
ec0_bridges_partial_safety_factors_es._set_bridges_partial_safety_factors(bridges_partial_safety_factors)

# Combination factors for road bridges (table AN.5 (table A2.1) )
bridge_combination_factors= bridge_factors.getCombinationFactors()
ec0_bridges_combination_factors_es._set_bridges_combination_factors(bridge_combination_factors)

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
        super().__init__(combGeneratorName= 'EC0_ES', factors= bridge_factors)
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
        warningMsg= className+'.'+methodName+'; has been DEPRECATED, specify the nature of the action usinf newSelfWeightAction or newDeadLoadAction or ...'
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
        if not code in bridges_partial_safety_factors: # Part. sfty fctrs not yet defnd
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

building_factors= loadCombinations.Factors()

# Partial safety factors for buildings.
buildings_partial_safety_factors= building_factors.getPartialSafetyFactors()


class BuildingCombGenerator(utils.CombGenerator):
    ''' Generate combinations corresponding to Eurocode 0 (Spanish annex).

    '''

    def __init__(self):
        ''' Constructor.
        '''
        super().__init__(combGeneratorName= 'EC0_ES', factors= building_factors)
