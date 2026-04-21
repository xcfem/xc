# -*- coding: utf-8 -*-
''' Definition of the variables that make up a reinforced concrete section 
with reinforcement symmetric in both directions (as usual in columns)
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"


import sys
import math
from materials.sections.fiber_section import column_main_reinforcement
from materials.sections.fiber_section import rc_rectangular_column_section
from materials.sections.fiber_section import rc_circular_section
from misc_utils import log_messages as lmsg
import geom

ColumnMainReinforcement= column_main_reinforcement.ColumnMainReinforcement
RCRectangularColumnSection= rc_rectangular_column_section.RCRectangularColumnSection
RCCircularSection= rc_circular_section.RCCircularSection


