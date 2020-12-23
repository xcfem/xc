# -*- coding: utf-8 -*-
# Home made test.

from __future__ import print_function
from __future__ import division

from actions.snow import SIA_snow

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

h0= 400

sk= SIA_snow.Sk(h0)
skTeor= max(((h0*h0/(350*350))+1)*0.4e3,0.9e3)
qk= SIA_snow.qkPont(h0,"normal")
qkTeor= 0.8*sk

ratio1= abs(sk-skTeor)/skTeor
ratio2= abs(qk-qkTeor)/qkTeor

'''
print("sk=", sk)
print("skTeor=", skTeor)
print("ratio1= ", ratio1)
print("qk=", qk)
print("qkTeor=", qkTeor)
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio1<1e-15) and (ratio2<1e-15)):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
