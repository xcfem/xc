# -*- coding: utf-8 -*-
''' Trivial test concering ReinfRow class: reinforcement row.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import math
from materials.sections.fiber_section import def_simple_RC_section

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

barDiameter= 10e-3 # Diameter of the reiforcement bar.
barArea= math.pi*(barDiameter/2.0)**2 # Area of the reinforcement bar.
spacing= 0.15 # spacing of reinforcement.
nBars= 10 # number of bars.
lateralCover= 0.035 # concrete cover for the bars at the extremities of the row.
width= nBars*spacing+2.0*lateralCover

# Reinforcement row.
row1= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, areaRebar= barArea, rebarsSpacing= spacing, width= width, nominalCover= 0.035, nominalLatCover= lateralCover)
area= row1.getAs()

ratio1= abs(row1.nRebars-nBars)
ratio2= abs(area-nBars*barArea)/(nBars*barArea)

'''
print(row1.nRebars)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) and (abs(ratio2)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
