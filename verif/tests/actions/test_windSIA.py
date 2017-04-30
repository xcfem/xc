# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from actions.wind import windSIA261
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.33 # Coeficiente de Poison
rho= 1.5 # Densidad



z= 10
catTerrain= "III"
qp0= 1.1


ch= windSIA261.Ch(z,catTerrain)
qp= windSIA261.qp(qp0,z,catTerrain)

chTeor= 1.6*(math.pow(z/450,0.23)+0.375)**2
ratio1= abs(ch-chTeor)/chTeor

# print "ch= ", ch
# print "chTeor= ", chTeor
# print "qp= ", qp


import os
fname= os.path.basename(__file__)
if (ratio1<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
