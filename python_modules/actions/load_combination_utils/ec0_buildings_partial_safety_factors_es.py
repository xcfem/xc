# -*- coding: utf-8 -*-
''' Partial safety factors for buildings according to Spanish Annex for Eurocode 0.'''

__author__= 'Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)'
__copyright__= 'Copyright 2025, LCPT and AO_O'
__license__= 'GPL'
__version__= '3.0'
__email__= 'l.pereztato@ciccp.es ana.ortega@ciccp.es'

import sys
import loadCombinations
from misc_utils import log_messages as lmsg

def _set_buildings_partial_safety_factors(partial_safety_factors):
    ''' Sets the partial safety factors corrsponding to buildings.

    :param partial_safety_factors: object that stores the partial safety 
                                   factors for buildings.
    '''
    #Partial safety factors for permanent actions
    ## Permanent loads.
    ### Eurocode 0. table A1.2(A). Design values of actions (EQU) (Set A)
    partial_safety_factors['permanent_equ_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0.9,1.1,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['variable_equ_set_a']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    ### Eurocode 0. table A1.2(B). Design values of actions (STR/GEO) (Set B)
    partial_safety_factors['permanent_str/geo_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0,1.35,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['variable_str/geo_set_b']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.5,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    ### Eurocode 0. table A1.2(C). Design values of actions (STR/GEO) (Set C)
    partial_safety_factors['permanent_str/geo_set_c']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(1.0,1.0,1,1), loadCombinations.SLSPartialSafetyFactors(1,1))
    partial_safety_factors['variable_str/geo_set_c']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,1.30,0,1), loadCombinations.SLSPartialSafetyFactors(0,1))
    ### Eurocode 0. table A1.3  Design values of actions for use in accidental and seismic combinations of actions.
    partial_safety_factors['accidentales']= loadCombinations.PartialSafetyFactors(loadCombinations.ULSPartialSafetyFactors(0,0,0,1), loadCombinations.SLSPartialSafetyFactors(0,0))
