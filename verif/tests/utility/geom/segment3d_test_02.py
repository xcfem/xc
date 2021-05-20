# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division
import xc_base
import geom
import math

A= geom.Pos3d(11.3722,5.09673,4.80346)
B= geom.Pos3d(11.3722,3.61173,4.80346)
C= geom.Pos3d(11.3722,3.81673,4.80346)
D= geom.Pos3d(11.3722,1.94173,4.80346)

a= geom.Segment3d(A,B)
b= geom.Segment3d(C,D)

A.logFileName= "/tmp/erase.log" # Ignore warning messages
p= a.getIntersectionWithSegment(b)[0]
A.logFileName= "clog" # Display warnings if any.
pRef= geom.Segment3d(B,C).getCenterOfMass()
ratio= p.dist2(pRef)

'''
print('p: '+str(p)) 
print('pRef: '+str(pRef)) 
print('ratio: '+str(ratio)) 
'''

import os
fname= os.path.basename(__file__)
if(abs(ratio)<1e-10):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
