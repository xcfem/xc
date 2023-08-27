# -*- coding: utf-8 -*-
''' Reinforced concrete section rough calculations test EC2 version.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections import rebar_family
from rough_calculations import ng_rc_section as rc

beton= EC2_materials.C30
steel= EC2_materials.S500B
A10_15= EC2_limit_state_checking.EC2RebarFamily(steel,10e-3,0.15,0.05)


section= rc.RCSection(A10_15,beton,1.0,0.3)
MR= section.getMR()
ratio1= (MR-50.19718696496735e3)/50.19718696496735e3
#VR= section.getVR(Nd= -6.17e3,Md= 15.56e3) # not implemented yet.
#ratio2= (VR-192.75882159515368e3)/192.75882159515368e3

'''
print('MR= ', MR/1e3)
print('ratio1= ', ratio1)
print('VR= ', VR/1e3)
print('ratio2= ', ratio2)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

