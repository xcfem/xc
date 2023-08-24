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
A10_05= EC2_limit_state_checking.EC2RebarFamily(steel= steel, diam= 10e-3, spacing= 0.05, concreteCover= 0.025)
A16_05= EC2_limit_state_checking.EC2RebarFamily(steel= steel, diam= 16e-3, spacing= 0.05, concreteCover= 0.025)
A20_05= EC2_limit_state_checking.EC2RebarFamily(steel= steel, diam= 20e-3, spacing= 0.05, concreteCover= 0.025)

## Section geometry.
b= 0.3
dp= 0.03
h= 0.5
rcSectionA= ng_rc_section.RCSection(tensionRebars= A10_05, concrete= concrete, b= b, h= h)
rcSectionB= ng_rc_section.RCSection(tensionRebars= A16_05, concrete= concrete, b= b, h= h)
rcSectionC= ng_rc_section.RCSection(tensionRebars= A20_05, concrete= concrete, b= b, h= h)

## Internal forces
Md= 300e3
Nd= -250e3

AsA= rcSectionA.getAs()
AsB= rcSectionB.getAs()
AsC= rcSectionC.getAs()

MRA= rcSectionA.getMR(Nd= Nd, Md= Md)
MRB= rcSectionB.getMR(Nd= Nd, Md= Md)
MRC= rcSectionC.getMR(Nd= Nd, Md= Md)

'''
print('AsA= ', rcSectionA.getAs()*1e4, 'cm2')
print('MRA= ', MRA/1e3, 'kN.m')
print('AsB= ', rcSectionB.getAs()*1e4, 'cm2')
print('MRB= ', MRB/1e3, 'kN.m')
print('AsC= ', rcSectionC.getAs()*1e4, 'cm2')
print('MRC= ', MRC/1e3, 'kN.m')
'''

error= math.sqrt((AsA-4.712388980384689e-4)**2+(AsB-12.063715789784803e-4)**2+(AsC-18.849555921538755e-4)**2)
error+= math.sqrt((MRA-84.11121388781129e3)**2+(MRB-222.5038243016057e3)**2+(MRC-330.0e3)**2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
