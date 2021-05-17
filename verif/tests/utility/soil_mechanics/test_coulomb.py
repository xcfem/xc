# -*- coding: utf-8 -*-
'''
   Verification test for the function ka_coulomb, from table 6.4 of the book
   "Principios de ingeniería de cimentaciones." Braja M. Das, página 353,
   and from the table 10.3 of the book Geotécnia y Cimientos II. Jiménez Salas,
   página 683.

   Verification test for the function kp_coulomb, with data taken from 
   https://civilengineeringbible.com/subtopics.php?i=8
'''
from __future__ import print_function
from geotechnics import earth_pressure
import math

ka1= earth_pressure.ka_coulomb(a= math.radians(0), b= math.radians(0),fi= math.radians(28), d= math.radians(0))
ka2= earth_pressure.ka_coulomb(a= math.radians(0), b= math.radians(0),fi= math.radians(30), d= math.radians(5))
ka3= earth_pressure.ka_coulomb(a= math.radians(0), b= math.radians(0),fi= math.radians(32), d= math.radians(10))
ka4= earth_pressure.ka_coulomb(a= math.radians(-20), b= math.radians(-20), fi= math.radians(40), d= math.radians(10))

kp1= earth_pressure.kp_coulomb(a= math.radians(0), b= math.radians(0),fi= math.radians(26), d= math.radians(0))
kp2= earth_pressure.kp_coulomb(a= math.radians(0), b= math.radians(0),fi= math.radians(30), d= math.radians(-22))
kp3= earth_pressure.kp_coulomb(a= math.radians(0), b= math.radians(10),fi= math.radians(32), d= math.radians(-17))
kp4= earth_pressure.kp_coulomb(a= math.radians(0), b= math.radians(-10),fi= math.radians(40), d= math.radians(-16))

ratio1= abs(ka1-0.3610)/0.3610 # Table 6.4 page 353
ratio2= abs(ka2-0.3189)/0.3189 # Table 6.4 page 353
ratio3= abs(ka3-0.2853)/0.2853 # Table 6.4 page 353
ratio4= abs(ka4-0.085)/0.085

ratio5= abs(kp1-2.561)/2.561
ratio6= abs(kp2-6.675)/6.675
ratio7= abs(kp3-10.492)/10.492
ratio8= abs(kp4-5.335)/5.335

'''
print("ka1= ",ka1)
print("ka2= ",ka2)
print("ka3= ",ka3)
print("ka4= ",ka4)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("kp1= ",kp1)
print("ratio5= ",ratio5)
print("kp2= ",kp2)
print("ratio6= ",ratio6)
print("kp3= ",kp3)
print("ratio7= ",ratio7)
print("kp4= ",kp4)
print("ratio8= ",ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-3) & (ratio2<1e-3) & (ratio3<1e-3) & (ratio4<3e-2) & (ratio5<1e-4) & (ratio6<1e-4) & (ratio7<1e-4) & (ratio8<1e-4):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+": ERROR.")
