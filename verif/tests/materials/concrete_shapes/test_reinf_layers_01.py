# -*- coding: utf-8 -*-
''' Trivial test concering reinforcement layer definition.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

from materials.sections.fiber_section import def_simple_RC_section

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
lateralCover= 0.035 # concrete cover for the bars at the extremities of the row.
width= nBarsA*spacing+2.0*lateralCover

# First row
barDiameterA= 10e-3 # Diameter of the reinforcement bar.
## Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameterA, rebarsSpacing= spacing, width= width, nominalCover= 0.035, nominalLatCover= lateralCover)
areaA= rowA.getAs()

# Second row
barDiameterB= 20e-3 # Diameter of the reinforcement bar.
## Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameterB, rebarsSpacing= spacing, width= width-spacing, nominalCover= 0.035, nominalLatCover= lateralCover+spacing/2.0)
areaB= rowB.getAs()
area= areaA+areaB

# Check number of bars.
ratio1= rowB.nRebars-(nBarsA-1)

# Define reinforcement layers.
reinfLayers= def_simple_RC_section.LongReinfLayers([rowA, rowB])

# Check total area.
ratio2= abs(reinfLayers.getAs()-area)/area



'''
print('number of rebars in row b: ', rowB.nRebars)
print(ratio1)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
