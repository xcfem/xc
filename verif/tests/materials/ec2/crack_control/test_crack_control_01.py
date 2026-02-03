# -*- coding: utf-8 -*-
''' Crack width calculation according to clause 7.3.4 of EC2:2004. 
    Test based on the example from: https://web.archive.org/web/20241008065525/http://calculocivil.com/es/ec2/fisura/calc
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections import rebar_family
from postprocess import limit_state_data as lsd

# Geometry.
b= 0.4 # Section width (m).
h= 0.3 # Section depth (m).
c= 35e-3 # Concrete cover (m).
rebarDiameter= 16e-3 # diameter of the bars.
rebarSpacing= 17e-2 # spacing of the bonded reinforcement.

# Materials.
concrete= EC2_materials.C25 # concrete.
steel= EC2_materials.S500B # reinforcing steel.

# Reinforcement.
rebarRow= rebar_family.FamNBars(steel= steel, n= 3, diam= rebarDiameter, spacing= rebarSpacing, concreteCover= c)


sg_max= 175.74e6 # maximum stress in the reinforcement immediately after
                 # formation of the crack.

# Crack control.
limitState= EC2_limit_state_checking.freqLoadsCrackControl # Crack control under frequent loads.
crackController= limitState.getController(shortTermLoading= False)

# maximum final crack spacing
k2= 0.5 # Bending (see EC2 clause 7.3.4).
As= rebarRow.getAs()
ratio0= abs(As-0.0006031857894892403)/0.0006031857894892403
## Effective concrete area.
x= 5.99e-2 # neutral axis depth.
d= h-c-rebarRow.getDiam()/2.0 # effective depth of the cross-section 
# hc_eff= crackController.EC2_hceff(h= h, d= d, x= x)
hc_eff= 2*(h-d)
Ac_eff= b*hc_eff
ro_eff= As/Ac_eff # effective reinforcement ratio.
# Compute maximum final crack spacing using equation 7.11 of EC2.
s_r_max= crackController.s_r_max(k2= k2, cover= c, reinfPhi= rebarRow.getDiam(), spacing= rebarSpacing, ro_eff= ro_eff)
ratio1= abs(s_r_max-274.1230178669006e-3)/274.1230178669006e-3
# Compute mean strain difference using equation 7.9 of EC2.
meanStrainDifference= crackController.meanStrainDifference(sigma_s= sg_max, steel= steel, concrete= concrete, ro_eff= ro_eff)
ratio2= abs(meanStrainDifference-0.0005535415351842659)/0.0005535415351842659
# Compute Wk
Wk= crackController.computeWk(sigma_s= sg_max, steel= steel, concrete= concrete, ro_eff= ro_eff, k2= k2, cover= c, reinfPhi= rebarRow.getDiam(), spacing= rebarSpacing)
ratio3= abs(Wk-0.15173847613938812e-3)/0.15173847613938812e-3

'''
print('Effective reinforcement ratio ro_eff= ', ro_eff)
print('Effective depth: d= ', d*1e2, 'cm')
print('Maximum final crack spacing s_r_max= ', s_r_max*1e3, 'mm')
print('ratio1= ', ratio1)
print('Mean strain difference: (eps_cm-eps_sm)= ', meanStrainDifference)
print('ratio2= ', ratio2)
print('Crack width: Wk= ', Wk*1e3, 'mm')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-6) and (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
