# -*- coding: utf-8 -*-
'''Computation of the ultimate shear force failure due to diagonal 
   compression in the web (Vu1) according to the clause 44.2.3.1 of EHE-08.'''

from __future__ import print_function

from materials.ehe import EHE_limit_state_checking
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

fck= 30e6 # concrete characteristic compressive strength.
fcd= fck/1.5 # design value of concrete compressive strength.
Ncd= 0 # Design value of axial force in concrete
b= 0.35 # section width.
h= 0.7 # section depth.
As= 0.0 # reinforcement area.
Ac= b*h-As # concrete section total area.
b0= b # net width of the element according to clause 40.3.5.
d= h-0.041 # effective depth.
Vu1= EHE_limit_state_checking.getVu1EHE08(fck= fck, fcd= fcd, Ncd= Ncd, Ac= Ac,b0= b0,d= d, alpha= math.radians(90), theta= math.radians(45))

ratio1= abs((Vu1-1.3839e6)/1.3839e6)

'''
print("Vu1= ",Vu1/1e3," kN")
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
  
