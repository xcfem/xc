# -*- coding: utf-8 -*-
''' Reinforced concrete materials as defined in fib Model Code 2010 '''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import math
from materials import concrete_base

# Concrete according to Model Code 2010.

def gamma_t0(t0):
    ''' Return the value of gamma_t0 according to expression 5.1-71b
        of fib Model Code 2010.

    :param t0: age of concrete at loading in days adjusted according 
               to Eqs. (5.1-73) and (5.1-85).
    '''
    return 1.0/(2.3+3.5/math.sqrt(t0))
    
