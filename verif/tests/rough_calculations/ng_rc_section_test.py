# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

'''
   Wall stability
'''
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from materials.sections import rebar_family
from rough_calculations import ng_rc_section as rc


beton= SIA262_materials.c30_37
acier= SIA262_materials.B500B
A10_15= SIA262_limit_state_checking.SIARebarFamily(acier,10e-3,0.15,0.05)


section= rc.RCSection(A10_15,beton,1.0,0.3)
MR= section.getMR()
VR= section.getVR(Nd= -6.17e3,Md= 15.56e3)
ratio1= (MR-50.2222855584e3)/50.2222855584e3
ratio2= (VR-230.473590755e3)/230.473590755e3

'''
print('MR= ', MR/1e3)
print('ratio1= ', ratio1)
print('VR= ', VR/1e3)
print('ratio2= ', ratio2)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11) and (abs(ratio2)<1e-11):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

