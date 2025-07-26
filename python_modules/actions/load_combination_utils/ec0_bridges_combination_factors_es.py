# -*- coding: utf-8 -*-
''' Combination factors for bridges according to Spanish Annex for Eurocode 0.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2025, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from misc_utils import log_messages as lmsg

def _set_bridges_combination_factors(combination_factors):
    ''' Sets the partial safety factors corresponding to bridges according to
        table AN.5 (table A2.1).

    :param combination_factors: object that stores the combination factors
                                for bridges.
    '''
    combination_factors.insert('permanent', loadCombinations.CombinationFactors(1,1,1))
    combination_factors.insert('road_traffic_loads_gr1a_trucks', loadCombinations.CombinationFactors(0.75,0.75,0.0))
    combination_factors.insert('road_traffic_loads_gr1a_udl', loadCombinations.CombinationFactors(0.40,0.40,0.0))
    combination_factors.insert('road_traffic_loads_gr1a_footway', loadCombinations.CombinationFactors(0.40,0.40,0.0))
    combination_factors.insert('road_traffic_loads_gr1b_single_axe', loadCombinations.CombinationFactors(0.0,0.75,0.0))
    combination_factors.insert('road_traffic_loads_gr2_horizontal_forces', loadCombinations.CombinationFactors(0.0,0.0,0.0))
    combination_factors.insert('road_traffic_loads_gr3_pedestrian_loads', loadCombinations.CombinationFactors(0.0,0.0,0.0))
    combination_factors.insert('road_traffic_loads_gr4_crowd_loading', loadCombinations.CombinationFactors(0.0,0.0,0.0))
    combination_factors.insert('road_traffic_loads_gr5_vertical_forces', loadCombinations.CombinationFactors(0.0,0.0,0.0)) # Special vehicles. Vertical forces.
    combination_factors.insert('road_traffic_loads_gr6_horizontal_forces', loadCombinations.CombinationFactors(0.0,0.0,0.0)) # Special vehicles. Horizontal forces.
    combination_factors.insert('road_bridge_construction', loadCombinations.CombinationFactors(1.0, 0.0, 1.0)) # Tabla AN.5 (tabla A2.1) - Factores de simultaneidad para puentes de carretera.
    combination_factors.insert('road_bridge_wind_persistent_situation', loadCombinations.CombinationFactors(0.6,0.2,0.0))
    combination_factors.insert('road_bridge_wind_during_execution', loadCombinations.CombinationFactors(0.8,0.0,0.0))
    combination_factors.insert('road_bridge_wind_aster', loadCombinations.CombinationFactors(1.0,0.0,0.0))
    combination_factors.insert('road_bridge_thermal', loadCombinations.CombinationFactors(0.6,0.6,0.5))
    combination_factors.insert('road_bridge_snow', loadCombinations.CombinationFactors(0.8,0.0,0.0))
    combination_factors.insert('road_bridge_hydrostatic_pressure', loadCombinations.CombinationFactors(1.0,1.0,1.0))
    combination_factors.insert('road_bridge_construction_loads', loadCombinations.CombinationFactors(1.0,0.0,1.0))
    # Combination factors for railway bridges according to table A2.3 of Eurocode 0.
    combination_factors.insert('LM71_alone_uls', loadCombinations.CombinationFactors(0.8,0.8,0.0))
    combination_factors.insert('SW/0_alone_uls', loadCombinations.CombinationFactors(0.8,0.8,0.0))
    combination_factors.insert('SW/2_uls', loadCombinations.CombinationFactors(0.0,1.0,0.0))
    combination_factors.insert('unloaded_train_uls', loadCombinations.CombinationFactors(1.0,0.0,0.0))
    combination_factors.insert('railway_bridge_construction', loadCombinations.CombinationFactors(1.0, 0.0, 1.0)) # Tabla A2.3 - Factores de simultaneidad para puentes de ferrocearril.
    combination_factors.insert('railway_bridge_wind_persistent_situation', loadCombinations.CombinationFactors(0.75,0.5,0.0))
    combination_factors.insert('railway_bridge_wind_aster', loadCombinations.CombinationFactors(1.0,0.0,0.0))
    combination_factors.insert('railway_bridge_thermal', loadCombinations.CombinationFactors(0.6,0.6,0.5))
    combination_factors.insert('railway_bridge_snow', loadCombinations.CombinationFactors(0.8,0.0,0.0))
    combination_factors.insert('railway_bridge_construction_loads', loadCombinations.CombinationFactors(1.0,0.0,1.0))
