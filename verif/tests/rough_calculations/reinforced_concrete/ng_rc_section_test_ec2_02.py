# -*- coding: utf-8 -*-
''' Reinforced concrete section rough calculations test EC2 version.
    Check that  the effect of compressive axial load is favourable.'''

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

concrete= EC2_materials.C30
steel= EC2_materials.S500B
A20_15= EC2_limit_state_checking.EC2RebarFamily(steel= steel, diam= 20e-3, spacing= 0.15, concreteCover= 0.04)

Nd= -77.00e3
Md= 811.53e3

section= rc.RCSection(tensionRebars= A20_15, concrete= concrete, b= 1.0, h= 1.0)
MRNd= section.getMR(Nd= Nd, Md= Md)
VRNd= section.getVR(Nd= Nd, Md= Md)
MR0= section.getMR(Nd= 0.0, Md= Md)
VR0= section.getVR(Nd= 0.0, Md= Md)

# Check that the effect of compressive axial load is favourable.
testOK= (MRNd>MR0) and (VRNd>VR0)

'''
print(' MR(Nd= '+str(Nd/1e3)+' kN)= ', MRNd/1e3, 'kN.m')
print(' VR(Nd= '+str(Nd/1e3)+' kN)= ', VRNd/1e3, 'kN')
print(' MR(Nd= 0 kN)= ', MR0/1e3, 'kN.m')
print(' VR(Nd= 0 kN)= ', VR0/1e3, 'kN')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

