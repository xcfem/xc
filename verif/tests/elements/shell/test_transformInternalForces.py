# -*- coding: utf-8 -*-
# Taken from a document of Ivan Dario in Scribd.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials import internal_forces
import math

n1= -11000
n2= -3000
n12= -4200

NP= xc.Vector(internal_forces.transformInternalForces([n1,n2,n12],math.radians(41)))
NPTeor= xc.Vector([-11715.8,-2284.18,3376.55])
ratio= (NP-NPTeor).Norm()/NPTeor.Norm()

# print "NP= ", NP
# print "ratio= ", ratio

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
