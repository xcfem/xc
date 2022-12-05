# -*- coding: utf-8 -*-
''' Checks the computation of the interaction diagram.
   Home made test.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import math
from materials.sections.fiber_section import def_simple_RC_section

# Define a reinforcement row.
rebarDiam= 20e-3 # Bar diameter expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.
refAsA= 7*rebarArea
reinfRowA= def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars=7, width= 1.0, nominalCover= 0.035)
nominalCoverA= reinfRowA.getNominalCover()


# Define a copy of the previous reinforcement row and change the diameter of
# the bars.
rebarDiam= 12e-3
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.
refAsB= 7*rebarArea
reinfRowB= reinfRowA.getCopy()
reinfRowB.setRebarDiameter(rebarDiam)
nominalCoverB= reinfRowB.getNominalCover()

# Check the results.
AsA= reinfRowA.getAs()
ratio1= abs(AsA-refAsA)/refAsA
AsB= reinfRowB.getAs()
ratio2= abs(AsB-refAsB)/refAsB
radiusIncrement= (reinfRowA.rebarsDiam-reinfRowB.rebarsDiam)/2.0
ratio3= abs((nominalCoverB-nominalCoverA)-radiusIncrement)/radiusIncrement

'''
print(reinfRowA.rebarsSpacing)
print(ratio1)
print(reinfRowB.rebarsSpacing)
print(ratio2)
print('A nominal cover: ', nominalCoverA*1e3, 'mm')
print('B nominal cover: ', nominalCoverB*1e3, 'mm')
print('radius increment: ', radiusIncrement*1e3, 'mm')
print(ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
