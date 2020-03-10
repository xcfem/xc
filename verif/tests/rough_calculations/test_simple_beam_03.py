# -*- coding: utf-8 -*-
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
beam.l= 2.5


#Loads
Q= -135e3 # N
d= 1.2
q= -8.1e3 # N/m
x1= (beam.l-d)/2.0


Md= beam.getBendingMomentUnderUniformLoad(q,beam.l/2.0)+beam.getBendingMomentUnderConcentratedLoad(Q,x1,beam.l/2.0)+beam.getBendingMomentUnderConcentratedLoad(Q,x1+d,beam.l/2.0)

Qd= beam.getShearUnderUniformLoad(q,beam.l/2.0)+beam.getShearUnderConcentratedLoad(Q,x1,beam.l/2.0)+beam.getShearUnderConcentratedLoad(Q,x1+d,beam.l/2.0)


MdTeor= -q*2.5**2/8.0-2*Q*2.5/4.0+Q*d/2

ratio1= (Md-MdTeor)/MdTeor

# print 'Md= ', Md/1e3, ' kN m/m'
# print 'MdTeor= ', MdTeor/1e3, ' kN m/m'
# print "ratio1= ", ratio1
# print 'Qd= ', Qd/1e3, ' kN m/m'

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11 and abs(Qd)<1e-11:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
