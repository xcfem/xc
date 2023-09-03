# -*- coding: utf-8 -*-
''' Check results of miminum longitudinal reinforcement of beams according to EC2.

Based on the example here: https://www.structuralguide.com/worked-example-singly-reinforced-beam-design-using-ec2/
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
b= .225 # beam width.
h= .45 # beam depth.
cover= 25e-3
stirrupsDiameter= 10e-3
mainBarsDiameter= 20e-3
effectiveCover= cover+stirrupsDiameter+mainBarsDiameter/2.0

# Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C

# Design bending moment
Mk= 60e3 

# EC2:2004 7.3.2 Minimum reinforcement areas: NOT CHECKED IN THE EXAMPLE.
Act= b*h/2.0
d= h-effectiveCover
# EC2:2004 9.2.1.1 Minimum reinforcement area for beams
AsMinBeams= EC2_limit_state_checking.getAsMinBeams(concrete, steel, h= h, z= 0.9*d, bt= b, d= d, nationalAnnex= None)

ratio1= abs(AsMinBeams-1.185e-4)/1.185e-4

'''
print('  Minimum reinforcement for beams (EC2:2004 9.2.1.1): ', AsMinBeams*1e6, 'mm2')
print('ratio1= ', ratio1)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

