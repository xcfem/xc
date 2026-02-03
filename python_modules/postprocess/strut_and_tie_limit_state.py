# -*- coding: utf-8 -*-
''' Utilities for strut and tie limit state checking.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.config import file_names as fn
from postprocess import limit_state_data as lsd

class StrutAndTieLimitStateData(lsd.ULS_LimitStateData):
    ''' Ultimate limit state data for strut and tie models.'''
    def __init__(self, designSituations= lsd.default_uls_design_situations):
        '''Constructor 

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        '''
        super(StrutAndTieLimitStateData,self).__init__(limitStateLabel= 'StrutAndTie', outputDataBaseFileName= fn.strutAndTieVerificationResultsFile, designSituations= designSituations)
        
# StrutAndTie limit state.
strutAndTieLimitState= StrutAndTieLimitStateData()
