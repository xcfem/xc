# -*- coding: utf-8 -*-
''' Trivial test of a rectangular section subject to bending with axial force.

Results were compared with those obtained from: https://calculocivil.com/eu/ec2/bending/calc
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

# Bending with axial force.
## Materials.
concrete= EC2_materials.C25
steel= EC2_materials.S500B
As1= 16.75e-4
As2= 0.0


## Geometry
b= 0.3
d= 0.47
dp= 0.03
h= 0.5

Nu, Mu= EC2_limit_state_checking.get_nu_mu(concrete= concrete, steel= steel, b= b, h= h, d= d, dp= dp, x= 0.2453, As1= As1, As2= As2)

error= math.sqrt((Nu-251.48323996861802e3)**2+(Mu-309.562343206904e3)**2)

'''
print('Nu= ', Nu/1e3, 'kN')
print('Mu= ', Mu/1e3, 'kN.m')
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
