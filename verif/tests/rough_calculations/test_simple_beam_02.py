# -*- coding: utf-8 -*-
from __future__ import print_function
from rough_calculations import ng_simple_beam as sb

''' From:
PrincetonX: CEE262.1x The Art of Structural Engineering: Bridges 
Week 5: Robert Maillart and Concrete Bridges > Structural Studies > Calculation Exercise 2: Bending Moment.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

beam= sb.SimpleBeam()
beam.l= 100 #ft

q= -(3.0+2.0) #k/ft; see sign convention on file ng_simple_beam.py

#Loads
M= 0.0
M= beam.getBendingMomentUnderUniformLoad(q,beam.l/2.0)

ratio1= (M-6250)/6250

# print("M= ", M, " k ft")
# print("ratio1= ", ratio1)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11:
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

