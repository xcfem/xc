# -*- coding: utf-8 -*-
from rough_calculations import ng_simple_beam as sb

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

beam= sb.SimpleBeam()
beam.l= 17.5

#Loads
RA= 0.0
x1= 2.75+0.09-0.8
x2= x1+0.8
x3= x2+1.6
x4= x3+1.6
x5= x4+1.6
x6= x5+0.8
RA+= beam.getReaction1UnderUniformLoadPartiallyDistributed(-20.0e3,0.0,x1)
RA+= beam.getReaction1UnderConcentratedLoad(-62.5e3,x2)
RA+= beam.getReaction1UnderConcentratedLoad(-62.5e3,x3)
RA+= beam.getReaction1UnderConcentratedLoad(-62.5e3,x4)
RA+= beam.getReaction1UnderConcentratedLoad(-62.5e3,x5)
RA+= beam.getReaction1UnderUniformLoadPartiallyDistributed(-20.0e3,x6,beam.l-x6)

ratio1= (RA-260.469714286e3)/260.469714286e3

# print "RA= ", RA/1e3, " kN"
# print "ratio1= ", ratio1


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

