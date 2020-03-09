# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
import sys
from materials.sia262 import SIA262_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

concrete= SIA262_materials.c45_55
#concrete.gmmC= 1.5
fcd= concrete.fcd()
fcdTeor= pow((-30e6/concrete.fck),(1.0/3.0))*concrete.fck/concrete.gmmC
ratio1= (fcd-fcdTeor)/fcdTeor

'''
print "fcd= ", fcd/1e6, " MPa"
print "fcdTeor= ", fcdTeor/1e6, " MPa"
print "ratio1= ", ratio1
'''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


