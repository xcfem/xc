# -*- coding: utf-8 -*-
#home cooked test

from rough_calculations import ng_cantilever as cv

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Wind according to SIA 261
Q= 7.32e3
EI= 732.9e3
beam= cv.Cantilever(210e9,3.49e-6,4.5)

delta= beam.getDeflectionUnderConcentratedLoad(Q,3.25,beam.l)
ratio1= (delta-0.180221124983)/0.180221124983

#print "EI= ",beam.EI()
#print "delta= ",delta

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
