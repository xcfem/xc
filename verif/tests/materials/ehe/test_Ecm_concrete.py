# -*- coding: utf-8 -*-
from __future__ import print_function
# Problem type

from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

fck= 25e6
gammac=1.5
concr= EHE_materials.EHEConcrete('HA',-fck,gammac)
fcm= concr.getFcm()*(-1)
ratio1= abs(fcm-33e6)/33e6
Ecm= concr.getEcm()
ratio2= abs(Ecm-27.3e9)/27.3e9

''' 
print("fck= ",fck)
print("fcm= ",fcm)
print("ratio1= ",ratio1)
print("Ecm= ",Ecm)
print("ratio2= ",ratio2)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) & (ratio2<2e-3)):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')


