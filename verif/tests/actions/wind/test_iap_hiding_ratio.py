# -*- coding: utf-8 -*-
''' Test computation of hiding ratios according to table 4.2-c of IAP-11.'''

from __future__ import print_function
from __future__ import division

from actions.wind import IAP_wind

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

ratio1= abs(IAP_wind.getHidingFactor(0.5, 0.1)-0.75)/0.75
ratio2= abs(IAP_wind.getHidingFactor(0.5, 1.0)-0.06)/0.06
ratio3= abs(IAP_wind.getHidingFactor(3, 0.3)-0.72)/0.72
ratio4= abs(IAP_wind.getHidingFactor(6, 0.1)-1.0)/1.0
ratio5= abs(IAP_wind.getHidingFactor(6, 1.0)-1.0)/1.0

'''
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
''' 


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12)and (abs(ratio5)<1e-12):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
