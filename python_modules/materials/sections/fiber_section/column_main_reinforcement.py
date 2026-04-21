# -*- coding: utf-8 -*-
''' ColumnMainReinforcement class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math

class ColumnMainReinforcement(object):
    ''' Main (longitudinal) rebars of a column.

    :ivar cover:          concrete clear cover of main reinforcement
    :ivar nRebarsWidth:   number of rebars in the width direction of the 
                      section (each face)
    :ivar areaRebarWidth: cross sectional area of each rebar in  width direction
    :ivar nRebarsDepth:   number of rebars in the depth direction of the 
                      section (each face)
    :ivar areaRebarDepth: cross sectional area of each rebar in  depth direction
    '''
    def __init__(self):
        self.cover= 0.030 
        self.nRebarsWidth= 5 
        self.areaRebarWidth= math.pi*(12e-3/2.0)**2

        self.nRebarsDepth= 2 
        self.areaRebarDepth= math.pi*(10e-3/2.0)**2
