# -*- coding: utf-8 -*-
''' Test computation of shrinkage and creep parameters according to Model Code
    10.

Based on
'''
from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.mc10 import MC10_td_concrete

concrete= EC2_materials.EC2Concrete("C20/25", -20e6, 1.5)

# concrete: concrete material.
# cement: 32.5N, 32.5R, 42.5N, 42.5R, 52.5N, 52.5R)
# h0: notional size of the section (2*A/u)
# T: average temperature over entire analysis period.
# RH: average relative humidity over entire analysis period.
# ts: time at start of drying (days).
# t0: loading age (days).
parameters= MC10_td_concrete.ShrinkageAndCreepParameters(concrete= concrete, cement= '42.5R', h0= 0.15, T= 21, RH= 50, ts= 14, t0= 7)

# Shrinkage related parameters for TDConcrete material.
opensees_epsba= parameters.get_opensees_epsba()
ratio1= abs(opensees_epsba+3.4264125205872555e-05)/3.4264125205872555e-05
opensees_epsbb= parameters.get_opensees_epsbb()
ratio2= abs(opensees_epsbb-1)
opensees_epsda= parameters.get_opensees_epsda()
ratio3= abs(opensees_epsda+0.0008529026767914643)/0.0008529026767914643
opensees_epsdb= parameters.get_opensees_epsdb()
ratio4= abs(opensees_epsdb-787.5)/787.5

# Creep related parameters for TDConcrete material.
opensees_phiba= parameters.get_opensees_phiba()
ratio5= abs(opensees_phiba-0.17468752155856918)/0.17468752155856918
opensees_phibb= parameters.get_opensees_phibb()
ratio6= abs(opensees_phibb-1)
opensees_phida= parameters.get_opensees_phida()
ratio7= abs(opensees_phida-3.651591802644834)/3.651591802644834
opensees_phidb= parameters.get_opensees_phidb()
ratio8= abs(opensees_phidb-504.5084971874737)/504.5084971874737
opensees_cem= parameters.get_opensees_cem()
ratio9= abs(opensees_cem-1)

'''
print('\nopensees_epsba= ', opensees_epsba, ratio1)
print('opensees_epsbb= ', opensees_epsbb, ratio2)
print('opensees_epsda= ', opensees_epsda, ratio3)
print('opensees_epsdb= ', opensees_epsdb, ratio4)

print('\nopensees_phiba= ', opensees_phiba, ratio5)
print('opensees_phibb= ', opensees_phibb, ratio6)
print('opensees_phida= ', opensees_phida, ratio7)
print('opensees_phidb= ', opensees_phidb, ratio8)
print('opensees_cem= ', opensees_cem, ratio9)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10) & (abs(ratio7)<1e-10) & (abs(ratio8)<1e-10) & (abs(ratio9)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
