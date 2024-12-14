# -*- coding: utf-8 -*-
''' Check calculation of punching shear resistance according to clause 6.4.4 
of Eurocode 2.

Example based on the results obtained using: 
https://eurocodeapplied.com/design/en1992/punching-shear-rectangular-internal-column
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

# Material.
concrete= EC2_materials.C30

dy= 0.35
rho_ly= 7.54e-4/dy
dz= 0.34
rho_lz= 7.54e-4/dz

vRdc= EC2_limit_state_checking.get_punching_shear_resistance_whitout_punching_shear_reinforcement(concrete= concrete, dy= dy, dz= dz, rho_ly= rho_ly, rho_lz= rho_lz, sigma_cp_y= 0.0, sigma_cp_z= 0.0, isFoundation= False, nationalAnnex= None)
vRd_max= EC2_limit_state_checking.get_maximum_punching_shear_capacity_on_column_periphery(concrete= concrete, nationalAnnex= None)

# maximum value of the shear stress adjacent to the column (u0).
vEd_0= 1.623e6
cf_0= vEd_0/vRd_max
ratio1= abs(cf_0-0.3842329545454545)/0.3842329545454545
# maximum value of the shear stress in the control perimeter (u1).
vEd_1= 0.409e6
cf_1= vEd_1/vRdc
ratio2= abs(cf_1-0.9126672377747078)/0.9126672377747078

'''
print('vRdc= ', vRdc/1e6, 'MPa')
print('cf_1= ', cf_1)
print('vRd_max= ', vRd_max/1e6, 'MPa')
print('cf_0= ', cf_0)
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-4) and (ratio2<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
