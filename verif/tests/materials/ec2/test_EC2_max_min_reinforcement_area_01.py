# -*- coding: utf-8 -*-
''' Computation of the minimumn and maximum reinforcement areas for
    a beam, according to expression 9.1N of EC2:2004.

    Test based on the example obtained from the web page:
    https://calculocivil.com/es/ec2/armado_minimo/calc
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

# Cross-section geometry
b= .3 # beam width.
h= .3 # beam depth.
effectiveCover= 35e-3
# Materials.
concrete= EC2_materials.C25
steel= EC2_materials.S500C

# Design bending moment
MEd= 70e3 

# EC2:2004 7.3.2 Minimum reinforcement areas
Act= b*h/2.0
d= h-effectiveCover
AsMinCrackControl= EC2_limit_state_checking.getAsMinCrackControl(concrete= concrete, reinfSteel= steel, h= h, Act= Act, sigmaC= 0.0)
refAsMinCrackControl= 0.92e-4
ratio0= abs(AsMinCrackControl-refAsMinCrackControl)/refAsMinCrackControl

# EC2:2004 9.2.1.1 Minimum reinforcement area for beams
# h: section depth.
# z: lever arm of internal forces (inner lever arm).
# bt: mean width of the tension zone
# d: effective depth.
AsMinBeams= EC2_limit_state_checking.getAsMinBeams(concrete= concrete, reinfSteel= steel, h= h, z= 0.8*h, bt= b, d= 0.9*h, nationalAnnex= 'Spain')
refAsMinBeams= 4500/24*10/3/434.78/1e4
ratio1= abs(AsMinBeams-refAsMinBeams)/refAsMinBeams
# EC2:2004 9.2.1.1 Maximum reinforcement area for beams
# b: section width.
# h: section depth.
AsMaxBeams= EC2_limit_state_checking.getAsMaxBeams(Ac= b*h)
refAsMaxBeams= 0.04*300*300/1e6
ratio2= abs(AsMaxBeams-refAsMaxBeams)/refAsMaxBeams

'''
print('  Crack control minimum reinforcement (EC2:2004 7.3.2): ', AsMinCrackControl*1e6, 'mm2')
print('Reference value: ', refAsMinCrackControl*1e6, 'mm2')
print('ratio0= ', ratio0)
print('Minimum reinforcement for beams (EC2:2004 9.2.1.1 -Spanish annex-): ', AsMinBeams*1e6, 'mm2')
print('Reference value: ', refAsMinBeams*1e6, 'mm2')
print('ratio1= ', ratio1)
print('Maximum reinforcement for beams (EC2:2004 9.2.1.1): ', AsMaxBeams*1e6, 'mm2')
print('Reference value: ', refAsMaxBeams*1e6, 'mm2')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-2) and (ratio1<1e-3) and (ratio2<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

