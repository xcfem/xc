# -*- coding: utf-8 -*-
# Home made test.

from __future__ import print_function
from __future__ import division
from actions.wind import SIA261_wind
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

z= 10
catTerrain= "III"
qp0= 1.1


ch= SIA261_wind.Ch(z,catTerrain)
qp= SIA261_wind.qp(qp0,z,catTerrain)

chTeor= 1.6*(math.pow(z/450,0.23)+0.375)**2
ratio1= abs(ch-chTeor)/chTeor

# print("ch= ", ch)
# print("chTeor= ", chTeor)
# print("qp= ", qp)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')
