# -*- coding: utf-8 -*-
''' Momento tope.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2023, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from rough_calculations import ng_rc_section

# Bending with axial force.

# Example 15.8 from the book.
## Materials.
concrete= EC2_materials.C25
steel= EC2_materials.S500B
A10_15= EC2_limit_state_checking.EC2RebarFamily(steel= steel, diam= 10e-3, spacing= 0.15, concreteCover= 0.025)

## Section geometry.
b= 0.3
dp= 0.03
h= 0.5
rcSection= ng_rc_section.RCSection(tensionRebars= A10_15, concrete= concrete, b= b, h= h)

## Internal forces
Md= 300e3
Nd= -250e3

Ap= rcSection.getCompressionReinforcementArea(dp= dp, Nd= Nd, Md= Md)
A= rcSection.getTensionReinforcementArea(Ap= Ap, dp= dp, Nd= Nd, Md= Md)
Up= Ap*steel.fyd()
U= A*steel.fyd()

error= math.sqrt((Ap)**2+(A-15.997484618067281e-4)**2)

'''
print('fcd= ', -concrete.fcd()/1e6, 'MPa')
print('d= ', rcSection.d())
print('dp= ', dp)
print('Ap= ',Ap*1e4, 'cm2')
print('Up= ',Up/1e3, 'kN')
print('A= ',A*1e4, 'cm2')
print('U= ',U/1e3, 'kN')
print('error= ', error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
