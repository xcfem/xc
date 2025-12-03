# -*- coding: utf-8 -*-
'''Shrinkage computation verification test.

Data for comparison obtained from:
https://eurocodeapplied.com/design/en1992/creep-shrinkage
'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials

concrete= EC2_materials.C30
concrete.cemType= 'N'   # class N cement
RH= 70  # ambient relative humidity(%)
t= 1e9 # age of the concrete.
ts= 7  #  age of the concrete at end of curing
Ac= 1.0 # concrete cross-sectional area 
u= 2 # part of cross-section perimeter exposed to drying
h0=2*Ac/u  # notional size of the member h0.

kh= concrete.getShrKh(h0)
ratio1= abs(kh-0.7)/0.7

Epscd0= concrete.getShrEpscd0(RH) # Basic drying shrinkage strain
ratio2= abs(Epscd0+36.20942640862215e-5)/-36.20942640862215e-5
Epsca= concrete.getShrEpsca(t) # Autogenous shrinkage strain
ratio3= abs(Epsca+5e-5)/5e-5
Epscs= concrete.getShrEpscs(t,ts,RH,h0) # Total shrinkage
ratio4= abs(Epscs+30.35e-5)/30.35e-5

'''
print('h0= ', h0, ' m')
print('kh= ', kh)
print('Basic drying shrinkage strain: ', str(Epscd0*1e5)+'e-5')
print('ratio2= ', ratio2)
print('Autogenous shrinkage strain: ', str(Epsca*1e5)+'e-5')
print('ratio3= ', ratio3)
print('Total shrinkage: ', str(Epscs*1e5)+'e-5')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

