# -*- coding: utf-8 -*-
# Home made test.

from __future__ import print_function
from __future__ import division
import sys
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from rough_calculations import ng_simple_bending_reinforcement

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

beton= SIA262_materials.c25_30
acier= SIA262_materials.B500A
acier.gammaS= 1.15 #EC2 takes 500/435
Dmax= 32e-3
b= 1.0
d= 0.74
h= 0.8
Vd= 0.371e6
Md= 0.552e6
Nd=0

rho= 0.33e-2
As= rho*h
VRd= SIA262_limit_state_checking.VuNoShearRebars(beton,acier,Nd,Md,As,b,d)
ratio1= abs(VRd-340.143003064e3)/340.143003064e3

'''
print("rho= ", rho, " m")
print("As= ", As*1e4, " cm2")
print("VRd= ", VRd/1e3, " kN")
print("ratio1= ", ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


