# -*- coding: utf-8 -*-
''' Combination factors for buildings according to Spanish Annex for Eurocode 0.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2025, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from misc_utils import log_messages as lmsg

def _set_buildings_combination_factors(combination_factors, altitude:float, cen_state:str= 'Spain'):
    ''' Sets the partial safety factors corrsponding to buildings according to
        table A1.1 Eurocode 0 (at present -2025-, the Spanish National Annex 
        has no information about buildings).

    :param combination_factors: object that stores the combination factors
                                for buildings.
    :param altitude: altitude of the roof (meters above sea level).
    :param cen_state: CEN member state.
    '''
    combination_factors.insert('permanent', loadCombinations.CombinationFactors(1,1,1))
    combination_factors.insert('cat_a_domestic_residential_areas', loadCombinations.CombinationFactors(0.7,0.5,0.3))
    combination_factors.insert('cat_b_office_areas', loadCombinations.CombinationFactors(0.7,0.5,0.3))
    combination_factors.insert('cat_c_congregation_areas', loadCombinations.CombinationFactors(0.7,0.7,0.6))
    combination_factors.insert('cat_D_shopping_areas', loadCombinations.CombinationFactors(0.7 , 0.7 , 0.6)) 
    combination_factors.insert('cat_E_storage_areas', loadCombinations.CombinationFactors(1.0 , 0.9 , 0.8 ))
    combination_factors.insert('cat_F_traffic_area_vehicle_weight_leq_30kN', loadCombinations.CombinationFactors(0.7 , 0.7 , 0.6))

    combination_factors.insert('cat_G_traffic_area_30kN_lt_vehicle weight_leq_160kN' , loadCombinations.CombinationFactors(0.7 , 0.5 , 0.3))
    if(cen_state in ['Finland', 'Iceland', 'Norway', 'Sweden']):
        combination_factors.insert('cat_H_snow_loads_on_building_roofs', loadCombinations.CombinationFactors(0.70 , 0.50 , 0.20))
    else:
        if(altitude>1000):
            combination_factors.insert('cat_H_snow_loads_on_building_roofs', loadCombinations.CombinationFactors(0.70 , 0.50 , 0.20))
        else:
            combination_factors.insert('cat_H_snow_loads_on_building_roofs', loadCombinations.CombinationFactors(0.50 , 0.20 , 0.0))
    combination_factors.insert('wind_loads_on_buildings', loadCombinations.CombinationFactors(0.6, 0.2, 0.0))
    combination_factors.insert('temperature_in_buildings', loadCombinations.CombinationFactors(0.6, 0.5, 0.0))
