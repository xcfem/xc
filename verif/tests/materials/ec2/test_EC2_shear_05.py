# -*- coding: utf-8 -*-
''' Example 6.4b from EC2 Worked Examples (rev A 31-03-2017)'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

# Geometry
bw= .15
h= .6
d= .55
Ac= bw*h
z= .5

# Passive reinforcement.
stirrups= EC2_materials.rebarsEC2['fi12']
stirrupsArea= stirrups['area']
numberOfLegs= 2
stirrupSpacing= .15
stirrupsSteel= EC2_materials.S500C

Asw= numberOfLegs*stirrupsArea # cross-sectional area of the shear reinforcement
fyd= stirrupsSteel.fyd()

# Concrete a)
concrete= EC2_materials.C30
#concrete.alfacc= 0.85 # coefficient taking account of long term effects on the compressive strength
nu= concrete.getShearStrengthReductionFactor()
theta= math.asin(math.sqrt(Asw*fyd/(bw*stirrupSpacing*nu*-concrete.fcd())))
VRdsA= EC2_limit_state_checking.getShearResistanceShearReinf(concrete= concrete, NEd= 0.0, Ac= Ac, bw= bw, Asw= Asw, s= stirrupSpacing, z= z, shearReinfSteel= stirrupsSteel, shearReinfAngle= math.pi/2.0, strutAngle= theta, nationalAnnex= None)
VRdsARef= 390.0862269964134e3
ratio1= abs(VRdsA-VRdsARef)/VRdsARef

# Concrete b)
concrete= EC2_materials.C60
#concrete.alfacc= 0.85 # coefficient taking account of long term effects on the compressive strength
nu= concrete.getShearStrengthReductionFactor()
theta= math.asin(math.sqrt(Asw*fyd/(bw*stirrupSpacing*nu*-concrete.fcd())))
VRdsB= EC2_limit_state_checking.getShearResistanceShearReinf(concrete= concrete, NEd= 0.0, Ac= Ac, bw= bw, Asw= Asw, s= stirrupSpacing, z= z, shearReinfSteel= stirrupsSteel, shearReinfAngle= math.pi/2.0, strutAngle= theta, nationalAnnex= None)
VRdsBRef= 583.9440401883879e3
ratio2= abs(VRdsB-VRdsBRef)/VRdsBRef

# # Concrete c) SOMETHING IS WRONG HERE:
# # fcd is NOT 51 MPa for fck= 90 MPa as written in the example. 
# concrete= EC2_materials.C90
# #concrete.alfacc= 0.85 # coefficient taking account of long term effects on the compressive strength
# nu= concrete.getShearStrengthReductionFactor()
# theta= math.asin(math.sqrt(Asw*fyd/(bw*stirrupSpacing*nu*-concrete.fcd())))
# VRdsC= EC2_limit_state_checking.getShearResistanceShearReinf(concrete= concrete, NEd= 0.0, Ac= Ac, bw= bw, Asw= Asw, s= stirrupSpacing, z= z, shearReinfSteel= stirrupsSteel, shearReinfAngle= math.pi/2.0, strutAngle= theta, nationalAnnex= None)
# VRdsCRef= 701e3
# ratio3= abs(VRdsC-VRdsCRef)/VRdsCRef

'''
print('Asw= ', Asw*1e6, 'mm2')
print('fyd= ', fyd/1e6, 'MPa')
print('VRdsA= ', VRdsA/1e3, 'kN')
print('ratio1= ', ratio1)

print('VRdsB= ', VRdsB/1e3, 'kN')
print('ratio2= ', ratio2)

# print('VRdsC= ', VRdsC/1e3, 'kN')
# print('ratio3= ', ratio3)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
