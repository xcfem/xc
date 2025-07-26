# -*- coding: utf-8 -*-
''' Partial safety factors for bridges according to Spanish Annex for Eurocode 0.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2025, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from misc_utils import log_messages as lmsg

def _set_bridges_partial_safety_factors(partial_safety_factors):
    ''' Sets the partial safety factors corrsponding to bridges.

    :param partial_safety_factors: object that stores the partial safety 
                                   factors for bridges.
    '''
    #Partial safety factors for permanent actions
    ## Permanent loads.
    ### Eurocode 0. Table AN.8 [table A2.4(A)]. Design values of actions (EQU) (Set A)
    partial_safety_factors['self_weight_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,1.1,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['dead_load_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,1.1,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['earth_pressure_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0,1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    ### Eurocode 0. Table AN.9 [table A2.4(B)]. Design values of actions (STR/GEO) (Set B)
    partial_safety_factors['permanent_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1,1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['self_weight_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0,1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['dead_load_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0, 1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['rheological_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0, 1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['friction_in_sliding_supports_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0, 1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['earth_pressure_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0, 1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['pore_water_pressure_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0, 1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['settlement_linear_analysis_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.20,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    partial_safety_factors['settlement_non_linear_analysis_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    # Partial safety factors for variable actions.
    ### Eurocode 0. Table AN.9 [table A2.4(B)]. Design values of actions (STR/GEO) (Set B)
    #### Traffic loads.
    partial_safety_factors['variable_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    partial_safety_factors['road_traffic_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    partial_safety_factors['pedestrian_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    partial_safety_factors['railway_traffic_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.45,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    #### Thermal actions.
    partial_safety_factors['thermal_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    #### Hydrostatic pressure
    partial_safety_factors['hydrostatic_pressure_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    #### Eurocode 7 variable load. Table AN.9 [table A2.4(B)]. Live load on backfill surfaces.
    partial_safety_factors['load_on_backfill_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    partial_safety_factors['construction_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.35,0,1), loadCombinations.SLSPartialSafetyFactors(0,0))

    ### Eurocode 0. Table AN.8 [table A2.4(A)]. Design values of actions (EQU) (Set A)
    partial_safety_factors['variable_set_a']= partial_safety_factors['variable_set_b']
    partial_safety_factors['road_traffic_set_a']= partial_safety_factors['road_traffic_set_b']
    partial_safety_factors['pedestrian_set_a']= partial_safety_factors['pedestrian_set_b']
    partial_safety_factors['railway_traffic_set_a']= partial_safety_factors['railway_traffic_set_b']
    #### Thermal actions.
    partial_safety_factors['thermal_set_a']= partial_safety_factors['thermal_set_b']
    #### Hydrostatic pressure
    partial_safety_factors['pore_water_pressure_set_a']= partial_safety_factors['pore_water_pressure_set_b']
    partial_safety_factors['hydrostatic_pressure_set_a']= partial_safety_factors['hydrostatic_pressure_set_b']
    #### Eurocode 7 variable load. Table AN.9 [table A2.4(B)]. Live load on backfill surfaces.
    partial_safety_factors['load_on_backfill_set_a']= partial_safety_factors['load_on_backfill_set_b']
    partial_safety_factors['construction_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.25,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))

    # Partial safety factors for accidental actions.
    partial_safety_factors['accidentales']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,0,1), loadCombinations.SLSPartialSafetyFactors(0,0))
