# -*- coding: utf-8 -*-
from rough_calculations import ng_simple_beam as sb

''' From:
PrincetonX: CEE262.1x The Art of Structural Engineering: Bridges 
Week 5: Robert Maillart and Concrete Bridges > Structural Studies > Calculation Exercise 2: Bending Moment.'''
beam= sb.SimpleBeam()
beam.l= 100 #ft

q= 3.0+2.0 #k/ft

#Loads
M= 0.0
M= beam.getBendingMomentUnderUniformLoad(q,beam.l/2.0)

ratio1= (M-6250)/6250

# print "M= ", M, " k ft"
# print "ratio1= ", ratio1


import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-11:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

