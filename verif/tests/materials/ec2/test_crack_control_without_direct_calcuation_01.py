# -*- coding: utf-8 -*-
''' Control of cracking without direct calculation accordingo to clause 7.3.3
of EC2:2004 part 1-1.'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "



from materials.ec2 import EC2_limit_state_checking

steelStress= 199.94e6
maxDiam04= EC2_limit_state_checking.getMaximumBarDiameterForCrackControl(steelStress= steelStress, wk= 0.4e-3)
ratio1= abs(maxDiam04-32.012e-3)/32.012e-3
spacing04= EC2_limit_state_checking.getMaximumBarSpacingForCrackControl(steelStress, wk= 0.4e-3)
ratio2= abs(spacing04-0.3)/0.3
maxDiam03= EC2_limit_state_checking.getMaximumBarDiameterForCrackControl(steelStress= steelStress, wk= 0.3e-3)
ratio3= abs(maxDiam03-25.0105e-3)/25.0105e-3
spacing03= EC2_limit_state_checking.getMaximumBarSpacingForCrackControl(steelStress, wk= 0.3e-3)
ratio4= abs(spacing03-0.250075)/0.250075

'''
print('maximum diameter (w_k= 0.4 mm): ', maxDiam04*1e3,'mm')
print('ratio1= ', ratio1)
print('maximum spacing (w_k= 0.4 mm): ', spacing04*1e3,'mm')
print('ratio2= ', ratio2)
print('maximum diameter (w_k= 0.3 mm): ', maxDiam03*1e3,'mm')
print('ratio3= ', ratio3)
print('maximum spacing (w_k= 0.3 mm): ', spacing03*1e3,'mm')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
