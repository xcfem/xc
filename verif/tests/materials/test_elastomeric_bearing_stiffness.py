# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test
# Computation of the stiffness of a rectangular elastomeric bearing.

from materials import bridge_bearings

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

G= 900e3 # Elastomeric bearing shear modulus.
a= 0.3 # Width of the bearing (parallel to bridge axis).
b= 0.3 # Length of the bearing (parallel to lintel axis).
e= 0.002 # Net thickness of the bearing (without steel plates).

neop= bridge_bearings.ElastomericBearing(G,a,b,e)
E= neop.getEbearing()
Kv= neop.getKvert()
Kh= neop.getKhoriz()
KrotationX= neop.getKrotationLintelAxis()
KrotationZ= neop.getKrotationLintelAxis()
KrotationY= neop.getKrotationVerticalAxis()

ratio1= abs( 600000000.0-E)/ 600000000.0
ratio2= abs(27000000000.0-Kv)/27000000000.0
ratio3= abs(40500000.0-Kh)/40500000.0
ratio4= abs(956971000.0-KrotationX)/956971000.0
ratio5= abs(956972000.0-KrotationZ)/956972000.0
ratio6= abs(510192.0-KrotationY)/510192.0


''' 
print("E= ",E)
print("ratio1= ",ratio1)
print("Kv= ",Kv)
print("ratio2= ",ratio2)
print("Kh= ",Kh)
print("ratio3= ",ratio3)
print("KrotationX= ",KrotationX)
print("ratio4= ",ratio4)
print("KrotationZ= ",KrotationZ)
print("ratio5= ",ratio5)
print("KrotationY= ",KrotationY)
print("ratio6= ",ratio6)
  '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-8) & (abs(ratio6)<1e-3):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
