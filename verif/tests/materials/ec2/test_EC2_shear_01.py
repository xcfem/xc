# -*- coding: utf-8 -*-
''' Example 6.3 from EC2 Worked Examples (rev A 31-03-2017)'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

bw= .1
h= .2
d= .175
Ac= bw*h
Nd= -5e6*Ac
concrete= EC2_materials.C40
fctd= concrete.fctd() # Design mean tensile strength
fctdRef= 1.6374499333992043e6 # See fctm expression in table 3.1 EC2:2004
ratio1= abs(fctd-fctdRef)/fctdRef

## Shear strength for cracked sections subjected to bending moment.
crackedVRdc= EC2_limit_state_checking.getShearResistanceCrackedNoShearReinf(concrete= concrete, NEd= Nd, Ac= Ac, Asl= 0.0, bw= bw, d= d, nationalAnnex= None)
crackedVRdcRef= 24.08173308974897e3
ratio2= abs(crackedVRdc-crackedVRdcRef)/crackedVRdcRef

## Shear strength for non-cracked sections subjected to bending moment.
I= 1/12.0*bw*h**3 # second moment of area.
S= h/2.0*bw*h/4.0 # first moment of area above and about the centroidal axis.
nonCrackedVRdc= EC2_limit_state_checking.getShearResistanceNonCrackedNoShearReinf(concrete= concrete, I=I, S= S, NEd= Nd, Ac= Ac, bw= bw)
nonCrackedVRdcRef= 43.95652792149196e3
ratio3= abs(nonCrackedVRdc-nonCrackedVRdcRef)/nonCrackedVRdcRef

'''
print('Nd= ', Nd/1e3, 'kN')
print('fctd= ', fctd/1e6, 'MPa')
print('ratio1= ', ratio1)
print('cracked VRdc= ', crackedVRdc/1e3, 'kN')
print('ratio2= ', ratio2)
print('non-cracked VRdc= ', nonCrackedVRdc/1e3, 'kN')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-12) and (ratio3<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
