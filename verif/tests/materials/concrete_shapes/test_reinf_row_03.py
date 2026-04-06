# -*- coding: utf-8 -*-
''' Trivial test concering ReinfRow class: reinforcement row.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import math
from materials.sections.fiber_section import def_simple_RC_section

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
barDiameter= 25e-3 # Diameter of the reinforcement bar.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= (nBarsA-1)*spacing+2.0*lateralCover+barDiameter

## First row
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)

## Second row
### Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)


ratio1= abs(rowA.nRebars-nBarsA)
ratio2= abs(rowA.latCover-0.0475)/0.0475
ratio3= abs(rowB.nRebars-(nBarsA-1))
ratio4= abs(rowB.latCover-0.1225)/0.1225

'''
print('\nArea row A: ', rowA.getAs())
print('Number of rebars row A: ', rowA.nRebars)
print('ratio1= ', ratio1)
print('Spacing row A: ', rowA.rebarsSpacing)
print('Lateral cover row A: ', rowA.latCover)
print('ratio2= ', ratio2)

print('\nArea row B: ', rowB.getAs())
print('Number of rebars row B: ', rowB.nRebars)
print('ratio3= ', ratio3)
print('Spacing row B: ', rowB.rebarsSpacing)
print('Lateral cover row B: ', rowB.latCover)
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) and (abs(ratio2)<1e-12) and (ratio3==0) and (abs(ratio4)<1e-12) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
