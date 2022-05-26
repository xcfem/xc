# -*- coding: utf-8 -*-
''' Example 6.5 from EC2 Worked Examples (rev A 31-03-2017)'''

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
h= .8
d= .75
Ac= bw*h
z= .675

# Materials
## Concrete
concrete= EC2_materials.C30

## Shear reinforcement
stirrupsSteel= EC2_materials.S450B
stirrups= EC2_materials.rebarsEC2['fi10']
stirrupsArea= stirrups['area']
stirrupsAngle= math.pi/4.0
numberOfLegs= 2
stirrupsSpacing= 0.15

Asw= numberOfLegs*stirrupsArea # cross-sectional area of the shear reinforcement

# maximum effective shear reinforcement according to clause 6.2.3 of EC2:2004
Asw_max= EC2_limit_state_checking.getMaximumEffectiveShearReinforcement(concrete, NEd= 0.0, Ac= Ac, bw= bw, s= stirrupsSpacing, shearReinfSteel= stirrupsSteel, shearReinfAngle= stirrupsAngle, nationalAnnex= None)
corr= concrete.getShearStrengthReductionFactor()/.616*0.85
Asw_maxRef= corr*7.4e6*bw*stirrupsSpacing/stirrupsSteel.fyd()
ratio1= abs(Asw_max-Asw_maxRef)/Asw_maxRef

# angle theta of simultaneous concrete – reinforcement steel collapse
optimumStrutAngle= EC2_limit_state_checking.getStrutAngleForSimultaneousCollapse(concrete, bw= bw, s= stirrupsSpacing, Asw= Asw, shearReinfSteel= stirrupsSteel, shearReinfAngle= stirrupsAngle)
optimumStrutAngleRef= math.radians(25.32151662757501)
ratio2= abs(optimumStrutAngle-optimumStrutAngleRef)/optimumStrutAngleRef

# design value of the shear resistance
VRds= EC2_limit_state_checking.getShearResistanceShearReinf(concrete= concrete, NEd= 0.0, Ac= Ac, bw= bw, Asw= Asw, s= stirrupsSpacing, z= z, shearReinfSteel= stirrupsSteel, shearReinfAngle= stirrupsAngle, strutAngle= optimumStrutAngle, nationalAnnex= None)
VRdsRef= 608.9415444951244e3
ratio3= abs(VRds-VRdsRef)/VRdsRef

# increase of tensile force in the longitudinal bars.
VEd= VRds # Ultimate shear force.
incTensileForce= EC2_limit_state_checking.getAdditionalTensileForceMainReinf(VEd= VRds, shearReinfAngle= stirrupsAngle, strutAngle= optimumStrutAngle)
incTensileForceRef= 339.0165663093328e3
ratio4= abs(incTensileForce-incTensileForceRef)/incTensileForceRef

'''
print('Asw_max= ', Asw_max*1e4, 'cm2')
print('Asw_maxRef= ', Asw_maxRef*1e4, 'cm2')
print('ratio1= ', ratio1)
print('Strut angle for simultaneous collapse: theta=', math.degrees(optimumStrutAngle))
print('ratio2= ', ratio2)
print('VRds= ', VRds/1e3, 'kN')
print('ratio3= ', ratio3)
print('Increase of tensile force in the longitudinal bars due to shear: ', incTensileForce/1e3, 'kN')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<.1) and (ratio2<1e-12) and (ratio3<1e-12) and (ratio4<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
