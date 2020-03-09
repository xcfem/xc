# -*- coding: utf-8 -*-

''' Verification test based on the EXAMPLE ACI 318-14 RC-BM-001 of
the program SAFE form Computers and Structures Inc.'''

 
from __future__ import division
from __future__ import print_function
from materials.aci import ACI_materials
from materials.aci import ACI_limit_state_checking as lsc
import os
from miscUtils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2019, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

concrete= ACI_materials.ACIConcrete(concreteName="C35",fck=-35e6,gammaC=1.0)
reinfSteel= ACI_materials.A615G60
reinfSteel.gammaS= 1.0

corbel= lsc.Corbel(concrete, reinfSteel, width= 0.3, thickness= 0.5, depth= 0.45, Asc= 1e-4, Ah= 849e-6)

Vd= 370e3
Nd= 75e3
av= 0.1

Vn_cap= corbel.getNominalVerticalLoadCapacity()
ratio1= abs(Vn_cap-824780.2536)/824780.2536
CF_v= corbel.getShearCapacityFactor(Vd)
ratio2= abs(CF_v-1.00330812893)/1.00330812893
Asc= corbel.getRequiredPrimaryReinforcement(Vd,Nd,av)
ratio3= abs(Asc-583.248024779e-6)/583.248024779e-6
Asc_min= corbel.getMinimumPrimaryReinforcement(Vd,Nd,av)
ratio4= abs(Asc_min-806.963855422e-6)/806.963855422e-6
Ah_min= corbel.getMinimumShearReinforcement(Vd,Nd,av)
ratio5= abs(Ah_min-283e-6)/283e-6

'''
print('Vn_cap= ', Vn_cap/1e3, 'kN')
print('ratio1= ', ratio1)
print('CF_v= ', CF_v)
print('ratio2= ', ratio2)
print('Asc= ', Asc*1e6,' mm2')
print('ratio3= ', ratio3)
print('Asc_min= ', Asc_min*1e6,' mm2')
print('ratio4= ', ratio4)
print('Ah_min= ', Ah_min*1e6,' mm2')
print('ratio5= ', ratio5)
'''

from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) and (ratio2<1e-12) and (ratio3<1e-12) and (ratio4<1e-12) and (ratio5<1e-12)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

