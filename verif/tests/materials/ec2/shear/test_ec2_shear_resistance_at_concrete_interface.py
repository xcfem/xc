# -*- coding: utf-8 -*-
''' Check shear resistance at concrete interface formulas.

Inspired on the verification example:
https://www.dlubal.com/en/downloads-and-information/examples-and-tutorials/verification-examples/001024
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

concrete= EC2_materials.C25
steel= EC2_materials.S500B

VEd= 800e3

NEd= 0.0 # axial force in the cross-section.
Asl= 39.44e-4 # area of the tensile reinforcement.
bw= 0.4 # web width.
d= 1.28 # effective depth.
Ac= (2.5*0.29)+bw*(1.35-0.29) # area of concrete cross-section.

# Design Value of Shear Resistance According to DIN EN 1992-1-1, 6.2.2 (1)
VRdc= EC2_limit_state_checking.getShearResistanceCrackedNoShearReinf(concrete= concrete, NEd= NEd, Ac= Ac, Asl= Asl, bw= bw, d= d, nationalAnnex= 'Germany')
vRdc= VRdc/bw/d
# In the reference example, when computing the vRdc value, there is a mistake
# in the value of rho_l, it is taken as 0.00703 instead of 0.007703 which is
# the correct value. The following expression computes the correct value.
vRdc_ref= 0.1*1.3953*math.pow(100.0*0.007703*25, 1/3.0)*1e6
ratio1= abs(vRdc-vRdc_ref)/vRdc_ref

# Design Shear Resistance at Interface Without Shear Reinforcement acc. to DIN EN 1992-1-1, (NDP) 6.2.5 (1)
vRdi= EC2_limit_state_checking.get_shear_resistance_at_concrete_interface(concrete= concrete, steel= steel, surfaceClassification= 'indented', sigma_n= 0.0, rho= 0.0, nationalAnnex= 'Germany', alpha= math.pi/2.0)
vRdi_ref= 0.5*1.1969831626736875e6
ratio2= abs(vRdi-vRdi_ref)/vRdi_ref

# Design Shear Resistance at Interface With Shear Reinforcement acc. to DIN EN 1992-1-1, (NDP) 6.2.5 (1)
rho= 10.44e-4
vRdi2= EC2_limit_state_checking.get_shear_resistance_at_concrete_interface(concrete= concrete, steel= steel, surfaceClassification= 'indented', sigma_n= 0.0, rho= rho, nationalAnnex= 'Germany', alpha= math.pi/2.0)
# Again there is an error in the expression used to compute of VRdi the 1.2
# factor that multiplies mu and sin(alpha) is incorrect, the expression (6.25)
# EN 1992-1-1 says:
# v_{Rdi} = c \cdot f_{ctd} + \mu \cdot \sigma_{n} + \rho \cdot f_{yd} \cdot (\mu \cdot \sin\alpha + \cos\alpha) \le 0.5 \cdot \nu \cdot f_{cd}
vRdi2_ref= vRdi_ref+rho*434.7826086956522e6*0.9
ratio3= abs(vRdi2-vRdi2_ref)/vRdi2_ref

'''
print('vRdc= ', vRdc/1e6, 'MPa')
print('ratio1= ', ratio1)
print('vRdi= ', vRdi/1e6, 'MPa')
print('vRdi_ref= ', vRdi_ref/1e6, 'MPa')
print('ratio2= ', ratio2)
print('vRdi2= ', vRdi2/1e6, 'MPa')
print('vRdi2_ref= ', vRdi2_ref/1e6, 'MPa')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-4) and (ratio2<1e-4) and (ratio3<1e-4)):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
