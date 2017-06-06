# -*- coding: utf-8 -*-
# Home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Obtención de las rigideces de un apoyo de neopreno rectangular.

from materials.sia263 import SIA263_limit_state_controllers as fc

lambdaMaxA= fc.lambdaMax(21)
lambdaMaxB= fc.lambdaMax(5)
lambdaMaxC= fc.lambdaMax(2.4)

ratio1= abs(lambdaMaxA-2)/2
ratio2= abs(lambdaMaxB-2.5)/2.5
ratio3= abs(lambdaMaxC-2.58666666667)/2.58666666667

''' 
print "lambdaMaxA= ", lambdaMaxA
print "ratio1= ",ratio1
print "lambdaMaxB= ", lambdaMaxB
print "ratio2= ",ratio2
print "lambdaMaxC= ", lambdaMaxC
print "ratio3= ",ratio3
  '''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-11):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
