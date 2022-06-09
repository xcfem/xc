# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to clause
    8.4.4 of EC2:2004. Home made test.'''

from __future__ import division
from __future__ import print_function

import os
import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

diameters= [8e-3, 10e-3, 12e-3, 16e-3, 20e-3, 25e-3, 32e-3, 40e-3]
#diameters= [12e-3]

# Define rebar controllers.
rebarControllerGood= EC2_limit_state_checking.RebarController(concreteCover= 25e-3, spacing= 150e-3, alphaCoefs=(1.0, None, 1.0, 1.0, 1.0), eta1= 1.0, compression= False)
rebarControllerPoor= EC2_limit_state_checking.RebarController(concreteCover= 25e-3, spacing= 150e-3,alphaCoefs=(1.0, None, 1.0, 1.0, 1.0), eta1= 0.7, compression= False)

# Materials
concrete= EC2_materials.C25
steel= EC2_materials.S500C

lbd_straight_good_cond= list()
lbd_straight_poor_cond= list()
lbd_bent_good_cond= list()
lbd_bent_poor_cond= list()
for d in diameters:
    tmp= rebarControllerGood.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'straight')
    tmp= math.ceil(tmp*1e2)*10
    lbd_straight_good_cond.append(tmp)
    tmp= rebarControllerPoor.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'straight')
    tmp= math.ceil(tmp*1e2)*10
    lbd_straight_poor_cond.append(tmp)
    tmp= rebarControllerGood.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'bent')
    tmp= math.ceil(tmp*1e2)*10
    lbd_bent_good_cond.append(tmp)
    tmp= rebarControllerPoor.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'bent')
    tmp= math.ceil(tmp*1e2)*10
    lbd_bent_poor_cond.append(tmp)

values= [lbd_straight_good_cond, lbd_straight_poor_cond,lbd_bent_good_cond,lbd_bent_poor_cond]
refValues= [[230, 320, 410, 600, 780, 1010, 1300, 1760], [330, 450, 580, 850, 1110, 1450, 1850, 2510], [320, 410, 490, 650, 810, 1010, 1300, 1760], [460, 580, 700, 930, 1160, 1450, 1850, 2510]]

err= 0.0 

for row1, row2 in zip(values, refValues):
    for v1, v2 in zip(row1, row2):
        err+=(v1-v2)**2
err= math.sqrt(err)

'''
print(values)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')







